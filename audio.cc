#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <cmath>
#include <vector>
#include <cstdint>
#include <algorithm>

static const int   SR  = 44100;
static const float PI2 = 2.0f * 3.14159265358979f;

static ALCdevice*  dev = nullptr;
static ALCcontext* ctx = nullptr;

static bool sfxEnabled   = true;
static bool musicEnabled = true;

enum { SFX_MOVE, SFX_ROTATE, SFX_LAND, SFX_CLEAR, SFX_TETRIS, SFX_LEVELUP, SFX_GAMEOVER, SFX_COUNT };
static ALuint sfxBuf[SFX_COUNT];
static ALuint sfxSrc[SFX_COUNT];
static ALuint musicBuf;
static ALuint musicSrc;

// ── PCM helpers ────────────────────────────────────────────────────────────────

static std::vector<int16_t> genNote(float hz, float secs, float vol = 0.35f,
                                     float atk = 0.005f, float rel = 0.05f) {
    int n = (int)(secs * SR);
    std::vector<int16_t> out(n);
    for (int i = 0; i < n; i++) {
        float t   = (float)i / SR;
        float env = 1.0f;
        if (t < atk) env = t / atk;
        float rs = secs - rel;
        if (rel > 0.0f && t > rs) env *= 1.0f - (t - rs) / rel;
        out[i] = (int16_t)(vol * env * 32767.0f * sinf(PI2 * hz * t));
    }
    return out;
}

static std::vector<int16_t> silence(float secs) {
    return std::vector<int16_t>((size_t)(secs * SR), 0);
}

static std::vector<int16_t> concat(std::initializer_list<std::vector<int16_t>> segs) {
    std::vector<int16_t> out;
    for (auto& s : segs) out.insert(out.end(), s.begin(), s.end());
    return out;
}

static std::vector<int16_t> mix2(const std::vector<int16_t>& a, const std::vector<int16_t>& b) {
    size_t n = std::max(a.size(), b.size());
    std::vector<int16_t> out(n, 0);
    for (size_t i = 0; i < a.size(); i++) { int32_t v = out[i] + a[i]; out[i] = (int16_t)std::max(-32767, std::min(32767, v)); }
    for (size_t i = 0; i < b.size(); i++) { int32_t v = out[i] + b[i]; out[i] = (int16_t)std::max(-32767, std::min(32767, v)); }
    return out;
}

// Frequency sweep (glide from hz1 to hz2)
static std::vector<int16_t> genSweep(float hz1, float hz2, float secs, float vol = 0.4f) {
    int n = (int)(secs * SR);
    std::vector<int16_t> out(n);
    double phase = 0.0;
    for (int i = 0; i < n; i++) {
        float t    = (float)i / SR;
        float hz   = hz1 + (hz2 - hz1) * t / secs;
        float env  = 1.0f - 0.4f * t / secs;
        phase     += PI2 * hz / SR;
        out[i]     = (int16_t)(vol * env * 32767.0f * sinf((float)phase));
    }
    return out;
}

// ── SFX synthesis ──────────────────────────────────────────────────────────────

static std::vector<int16_t> makeSFX_Move() {
    return genNote(880.0f, 0.04f, 0.22f, 0.002f, 0.035f);
}

static std::vector<int16_t> makeSFX_Rotate() {
    // Two-tone blip
    return concat({ genNote(660.0f, 0.04f, 0.18f, 0.003f, 0.03f),
                    genNote(990.0f, 0.05f, 0.18f, 0.003f, 0.04f) });
}

static std::vector<int16_t> makeSFX_Land() {
    return genNote(130.0f, 0.18f, 0.55f, 0.002f, 0.15f);
}

static std::vector<int16_t> makeSFX_Clear() {
    return genSweep(380.0f, 860.0f, 0.22f, 0.48f);
}

static std::vector<int16_t> makeSFX_Tetris() {
    // Three ascending chord stabs
    auto c1 = genNote(523.25f, 0.13f, 0.35f, 0.005f, 0.08f);
    auto c2 = genNote(659.25f, 0.13f, 0.35f, 0.005f, 0.08f);
    auto c3 = genNote(783.99f, 0.13f, 0.35f, 0.005f, 0.08f);
    auto stab1 = mix2(c1, c2);
    stab1 = mix2(stab1, c3);

    auto d1 = genNote(587.33f, 0.13f, 0.35f, 0.005f, 0.08f);
    auto d2 = genNote(739.99f, 0.13f, 0.35f, 0.005f, 0.08f);
    auto d3 = genNote(880.00f, 0.13f, 0.35f, 0.005f, 0.08f);
    auto stab2 = mix2(d1, d2);
    stab2 = mix2(stab2, d3);

    auto e1 = genNote(659.25f, 0.20f, 0.35f, 0.005f, 0.15f);
    auto e2 = genNote(830.61f, 0.20f, 0.35f, 0.005f, 0.15f);
    auto e3 = genNote(987.77f, 0.20f, 0.35f, 0.005f, 0.15f);
    auto stab3 = mix2(e1, e2);
    stab3 = mix2(stab3, e3);

    return concat({ stab1, stab2, stab3 });
}

static std::vector<int16_t> makeSFX_LevelUp() {
    // Ascending arpeggio: C5 E5 G5 C6
    return concat({
        genNote(523.25f, 0.10f, 0.38f, 0.005f, 0.07f),
        genNote(659.25f, 0.10f, 0.38f, 0.005f, 0.07f),
        genNote(783.99f, 0.10f, 0.38f, 0.005f, 0.07f),
        genNote(1046.5f, 0.22f, 0.38f, 0.005f, 0.18f),
    });
}

static std::vector<int16_t> makeSFX_GameOver() {
    // Descending minor chord sequence
    auto make_chord = [](float r, float m, float b, float secs) {
        return mix2(mix2(genNote(r, secs, 0.28f, 0.01f, secs * 0.4f),
                         genNote(m, secs, 0.28f, 0.01f, secs * 0.4f)),
                    genNote(b, secs, 0.28f, 0.01f, secs * 0.4f));
    };
    return concat({
        make_chord(659.25f, 523.25f, 392.00f, 0.32f),
        make_chord(587.33f, 466.16f, 349.23f, 0.32f),
        make_chord(523.25f, 415.30f, 311.13f, 0.55f),
    });
}

// ── Korobeiniki melody synthesis ───────────────────────────────────────────────

// Music note: square-wave harmonics + vibrato for warm chiptune character
static std::vector<int16_t> mN(float hz, float dur) {
    int n = (int)(dur * SR);
    std::vector<int16_t> out(n);
    const float vol = 0.24f;
    const float atk = 0.010f;
    const float rel = 0.07f;
    double phase = 0.0;
    for (int i = 0; i < n; i++) {
        float t = (float)i / SR;
        float env = 1.0f;
        if (t < atk) env = t / atk;
        float rs = dur - rel;
        if (rel > 0.0f && t > rs) env *= 1.0f - (t - rs) / rel;
        // Ramp vibrato in after 40 ms (1.2% depth, 5.5 Hz rate)
        float vd = (t > 0.04f) ? std::min(1.0f, (t - 0.04f) / 0.04f) * 0.012f : 0.0f;
        float vib = vd * sinf(PI2 * 5.5f * t);
        phase += PI2 * (double)(hz * (1.0f + vib)) / SR;
        float p = (float)phase;
        // Odd harmonics → warm square-ish tone
        float s = 0.60f * sinf(p) + 0.25f * sinf(3.0f * p)
                + 0.10f * sinf(5.0f * p) + 0.05f * sinf(7.0f * p);
        out[i] = (int16_t)(vol * env * 32767.0f * s);
    }
    return out;
}

// Bass note: sawtooth harmonics for warm low-end depth
static std::vector<int16_t> mB(float hz, float dur) {
    int n = (int)(dur * SR);
    std::vector<int16_t> out(n);
    const float vol = 0.19f;
    const float atk = 0.004f;
    const float rel = 0.10f;
    double phase = 0.0;
    for (int i = 0; i < n; i++) {
        float t = (float)i / SR;
        float env = 1.0f;
        if (t < atk) env = t / atk;
        float rs = dur - rel;
        if (rel > 0.0f && t > rs) env *= 1.0f - (t - rs) / rel;
        phase += PI2 * (double)hz / SR;
        float p = (float)phase;
        // Sawtooth approximation (fundamental + falling harmonics), normalized
        float s = (sinf(p) + 0.500f * sinf(2.0f * p)
                 + 0.333f * sinf(3.0f * p) + 0.250f * sinf(4.0f * p)) / 2.083f;
        out[i] = (int16_t)(vol * env * 32767.0f * s);
    }
    return out;
}

static std::vector<int16_t> buildMusic() {
    // 144 BPM
    const float q  = 60.0f / 144.0f;
    const float e  = q / 2.0f;
    const float h  = q * 2.0f;
    const float dq = q * 1.5f;

    // Melody frequencies
    const float E5 = 659.25f, B4 = 493.88f, C5 = 523.25f, D5 = 587.33f;
    const float A4 = 440.00f, F5 = 698.46f, A5 = 880.00f, G5 = 783.99f;

    // Bass frequencies (octaves 2–3)
    const float E2 = 82.41f,  A2 = 110.00f, B2 = 123.47f, Fs2 = 92.50f;
    const float G2 = 98.00f,  C3 = 130.81f, D3 = 146.83f, E3 = 164.81f;

    // ── Melody ─────────────────────────────────────────────────
    std::vector<int16_t> melody;
    auto app = [&](std::vector<int16_t> seg) { melody.insert(melody.end(), seg.begin(), seg.end()); };

    // Part A
    app(mN(E5,q)); app(mN(B4,e)); app(mN(C5,e));
    app(mN(D5,q)); app(mN(C5,e)); app(mN(B4,e));
    app(mN(A4,q)); app(mN(A4,e)); app(mN(C5,e));
    app(mN(E5,q)); app(mN(D5,e)); app(mN(C5,e));
    app(mN(B4,dq)); app(mN(C5,e));
    app(mN(D5,q));  app(mN(E5,q));
    app(mN(C5,q));  app(mN(A4,q)); app(mN(A4,h));

    // Part B
    app(mN(D5,q)); app(mN(F5,q));
    app(mN(A5,dq)); app(mN(G5,e));
    app(mN(F5,q));
    app(mN(E5,dq)); app(mN(C5,e));
    app(mN(E5,q));
    app(mN(D5,e)); app(mN(C5,e));
    app(mN(B4,q));
    app(mN(B4,e)); app(mN(C5,e));
    app(mN(D5,e)); app(silence(e));
    app(mN(E5,q)); app(mN(C5,q));
    app(mN(A4,q)); app(mN(A4,h));

    // ── Bass (oom-pah: root on beats 1&3, fifth on beats 2&4) ──
    std::vector<int16_t> bass;
    auto appB = [&](std::vector<int16_t> seg) { bass.insert(bass.end(), seg.begin(), seg.end()); };

    // Part A
    appB(mB(E3,q)); appB(mB(B2,q)); appB(mB(E3,q));  appB(mB(B2,q));  // M1 Em
    appB(mB(A2,q)); appB(mB(E2,q)); appB(mB(A2,q));  appB(mB(E2,q));  // M2 Am
    appB(mB(B2,q)); appB(mB(Fs2,q)); appB(mB(B2,q)); appB(mB(Fs2,q)); // M3 B
    appB(mB(A2,q)); appB(mB(E2,q)); appB(mB(A2,h));                   // M4 Am

    // Part B
    appB(mB(D3,q)); appB(mB(A2,q)); appB(mB(D3,q)); appB(mB(A2,q));  // M5 Dm
    appB(mB(C3,q)); appB(mB(G2,q)); appB(mB(C3,q)); appB(mB(G2,q));  // M6 C
    appB(mB(G2,q)); appB(mB(D3,q)); appB(mB(G2,q)); appB(mB(D3,q));  // M7 G
    appB(mB(A2,q)); appB(mB(E2,q)); appB(mB(A2,q)); appB(mB(A2,h)); // M8 Am

    size_t len = std::max(melody.size(), bass.size());
    melody.resize(len, 0);
    bass.resize(len, 0);
    return mix2(melody, bass);
}

// ── OpenAL lifecycle ───────────────────────────────────────────────────────────

static void loadBuf(ALuint buf, const std::vector<int16_t>& data) {
    alBufferData(buf, AL_FORMAT_MONO16, data.data(),
                 (ALsizei)(data.size() * sizeof(int16_t)), SR);
}

void Audio_Init() {
    dev = alcOpenDevice(nullptr);
    if (!dev) return;
    ctx = alcCreateContext(dev, nullptr);
    if (!ctx) { alcCloseDevice(dev); dev = nullptr; return; }
    alcMakeContextCurrent(ctx);

    alGenBuffers(SFX_COUNT, sfxBuf);
    alGenSources(SFX_COUNT, sfxSrc);
    alGenBuffers(1, &musicBuf);
    alGenSources(1, &musicSrc);

    loadBuf(sfxBuf[SFX_MOVE],    makeSFX_Move());
    loadBuf(sfxBuf[SFX_ROTATE],  makeSFX_Rotate());
    loadBuf(sfxBuf[SFX_LAND],    makeSFX_Land());
    loadBuf(sfxBuf[SFX_CLEAR],   makeSFX_Clear());
    loadBuf(sfxBuf[SFX_TETRIS],  makeSFX_Tetris());
    loadBuf(sfxBuf[SFX_LEVELUP], makeSFX_LevelUp());
    loadBuf(sfxBuf[SFX_GAMEOVER],makeSFX_GameOver());

    for (int i = 0; i < SFX_COUNT; i++)
        alSourcei(sfxSrc[i], AL_BUFFER, sfxBuf[i]);

    loadBuf(musicBuf, buildMusic());
    alSourcei(musicSrc, AL_BUFFER,  musicBuf);
    alSourcei(musicSrc, AL_LOOPING, AL_TRUE);
    alSourcef(musicSrc, AL_GAIN,    0.65f);
    alSourcePlay(musicSrc);
}

void Audio_Shutdown() {
    if (!ctx) return;
    alSourceStopv(SFX_COUNT, sfxSrc);
    alSourceStop(musicSrc);
    alDeleteSources(SFX_COUNT, sfxSrc);
    alDeleteSources(1, &musicSrc);
    alDeleteBuffers(SFX_COUNT, sfxBuf);
    alDeleteBuffers(1, &musicBuf);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(ctx);
    alcCloseDevice(dev);
}

// ── Public API ─────────────────────────────────────────────────────────────────

static void playSFX(int id) {
    if (!sfxEnabled || !ctx) return;
    alSourceStop(sfxSrc[id]);
    alSourcePlay(sfxSrc[id]);
}

void Audio_PlayMove()     { playSFX(SFX_MOVE); }
void Audio_PlayRotate()   { playSFX(SFX_ROTATE); }
void Audio_PlayLand()     { playSFX(SFX_LAND); }

void Audio_PlayLineClear(int lines) {
    if (!sfxEnabled || !ctx) return;
    if (lines >= 4) {
        alSourceStop(sfxSrc[SFX_TETRIS]);
        alSourcePlay(sfxSrc[SFX_TETRIS]);
    } else {
        alSourceStop(sfxSrc[SFX_CLEAR]);
        alSourcePlay(sfxSrc[SFX_CLEAR]);
    }
}

void Audio_PlayLevelUp()  { playSFX(SFX_LEVELUP); }
void Audio_PlayGameOver() { playSFX(SFX_GAMEOVER); }

void Audio_StartMusic() {
    if (!musicEnabled || !ctx) return;
    alSourceStop(musicSrc);
    alSourcePlay(musicSrc);
}

void Audio_StopMusic() {
    if (!ctx) return;
    alSourceStop(musicSrc);
}

void Audio_SetMusicEnabled(bool on) {
    musicEnabled = on;
    if (!on) Audio_StopMusic();
    else     Audio_StartMusic();
}

void Audio_SetSFXEnabled(bool on) { sfxEnabled = on; }
bool Audio_GetMusicEnabled()      { return musicEnabled; }
bool Audio_GetSFXEnabled()        { return sfxEnabled; }
