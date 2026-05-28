#pragma once

void Audio_Init();
void Audio_Shutdown();

void Audio_PlayMove();
void Audio_PlayRotate();
void Audio_PlayLand();
void Audio_PlayLineClear(int lines);
void Audio_PlayLevelUp();
void Audio_PlayGameOver();

void Audio_StartMusic();
void Audio_StopMusic();
void Audio_SetMusicEnabled(bool on);
void Audio_SetSFXEnabled(bool on);
bool Audio_GetMusicEnabled();
bool Audio_GetSFXEnabled();
