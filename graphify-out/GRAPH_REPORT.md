# Graph Report - .  (2026-05-28)

## Corpus Check
- Corpus is ~13,924 words - fits in a single context window. You may not need a graph.

## Summary
- 59 nodes · 82 edges · 13 communities (6 shown, 7 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 4 edges (avg confidence: 0.82)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Block Physics & Ghost Piece|Block Physics & Ghost Piece]]
- [[_COMMUNITY_Input Handling & Init|Input Handling & Init]]
- [[_COMMUNITY_Block Rotation System|Block Rotation System]]
- [[_COMMUNITY_Line Clear & Animation|Line Clear & Animation]]
- [[_COMMUNITY_Block Spawning & Gravity|Block Spawning & Gravity]]
- [[_COMMUNITY_Board & UI Rendering|Board & UI Rendering]]
- [[_COMMUNITY_Text Rendering|Text Rendering]]
- [[_COMMUNITY_Color Utilities|Color Utilities]]
- [[_COMMUNITY_Game State|Game State]]
- [[_COMMUNITY_Score Float Text|Score Float Text]]
- [[_COMMUNITY_Cell Type Access|Cell Type Access]]
- [[_COMMUNITY_Point Data Type|Point Data Type]]
- [[_COMMUNITY_Raster Font|Raster Font]]

## God Nodes (most connected - your core abstractions)
1. `key_movement` - 12 edges
2. `main` - 10 edges
3. `Cell::draw` - 9 edges
4. `shift` - 9 edges
5. `display` - 8 edges
6. `rotate` - 6 edges
7. `timedDrop` - 5 edges
8. `drawFallingBlock` - 5 edges
9. `spawnNextBlock` - 5 edges
10. `afterFlash` - 5 edges

## Surprising Connections (you probably didn't know these)
- `currBlockPoints 5-element anchor invariant` --rationale_for--> `shift`  [EXTRACTED]
  CLAUDE.md → block.cc
- `Ghost piece / preview drop shadow` --conceptually_related_to--> `drawPreviewDrop`  [EXTRACTED]
  CLAUDE.md → block.cc
- `blockType char encoding scheme` --rationale_for--> `Cell::draw`  [EXTRACTED]
  CLAUDE.md → Cell.cc
- `currBlockPoints 5-element anchor invariant` --rationale_for--> `rotate`  [EXTRACTED]
  CLAUDE.md → block.cc
- `Ghost piece / preview drop shadow` --conceptually_related_to--> `drawGhostCell`  [EXTRACTED]
  CLAUDE.md → GLwindow.cc

## Hyperedges (group relationships)
- **GLUT Three-Timer Game Loop** — blockfall_fps, blockfall_timedrop, blockfall_key_movement [EXTRACTED 1.00]
- **Block Movement and Validation System** — block_shift, block_rotate, block_drop, block_validbounds, block_validboundsrot, blockfall_currblockpoints [INFERRED 0.95]
- **Cell Rendering Pipeline** — cell_draw, glwindow_drawcell, glwindow_drawemptycell, glwindow_drawghostcell, glwindow_set_rgb [INFERRED 0.95]

## Communities (13 total, 7 thin omitted)

### Community 0 - "Block Physics & Ghost Piece"
Cohesion: 0.23
Nodes (12): drawPreviewDrop, drop, validBounds, currBlockPoints (active block vector), DAS (Delayed Auto Shift), endLockPulse, fps timer callback, key_movement (+4 more)

### Community 1 - "Input Handling & Init"
Cohesion: 0.22
Nodes (9): char_keys, main, process_Normal_Keys, setDisp, special_up, Cell class, makeRasterFont, printString (+1 more)

### Community 2 - "Block Rotation System"
Cohesion: 0.22
Nodes (9): LowerLeft, rotate, validBoundsRot, Cell::draw, currBlockPoints 5-element anchor invariant, blockType char encoding scheme, Ghost piece / preview drop shadow, drawEmptyCell (+1 more)

### Community 3 - "Line Clear & Animation"
Cohesion: 0.25
Nodes (8): checkClearRow, checkRow, clearRow, findFullRows, afterFlash, endTetrisFlash, Cascade animation after row clear, Level system (speed scaling)

### Community 4 - "Block Spawning & Gravity"
Cohesion: 0.52
Nodes (7): genBlocks, shift, createBlock, drawFallingBlock, restartGame, spawnNextBlock, Cell::setType

### Community 5 - "Board & UI Rendering"
Cohesion: 0.43
Nodes (7): drawPreview, board (global game state), display, displayGameOver, displaySettings, drawCell, set_RGB

## Knowledge Gaps
- **23 isolated node(s):** `drawEmptyCell`, `RenderString`, `reshape`, `RGB`, `GameState enum` (+18 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **7 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `key_movement` connect `Block Physics & Ghost Piece` to `Input Handling & Init`, `Block Rotation System`, `Line Clear & Animation`, `Block Spawning & Gravity`?**
  _High betweenness centrality (0.332) - this node is a cross-community bridge._
- **Why does `main` connect `Input Handling & Init` to `Block Physics & Ghost Piece`, `Block Spawning & Gravity`, `Board & UI Rendering`?**
  _High betweenness centrality (0.269) - this node is a cross-community bridge._
- **Why does `spawnNextBlock` connect `Block Spawning & Gravity` to `Block Physics & Ghost Piece`, `Line Clear & Animation`?**
  _High betweenness centrality (0.158) - this node is a cross-community bridge._
- **What connects `drawEmptyCell`, `RenderString`, `reshape` to the rest of the system?**
  _23 weakly-connected nodes found - possible documentation gaps or missing edges._