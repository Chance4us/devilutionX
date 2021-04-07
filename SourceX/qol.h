/**
 * @file qol.h
 *
 * Quality of life features
 */
#pragma once

#include "engine.h"

namespace devilution {

void FreeQol();
void InitQol();
void DrawMonsterHealthBar(CelOutputBuffer out);
void DrawXPBar(CelOutputBuffer out);
void AutoGoldPickup(int pnum);

extern BYTE ItemAnimLs[];
extern BYTE *itemanims[ITEMTYPES];

void AltPressed(bool pressed);
bool IsGeneratingLabels();
bool IsLabelHighlighted();
void UpdateLabelOffsets(CelOutputBuffer out, BYTE *dst, int width);
void GenerateLabelOffsets(CelOutputBuffer out);
void AddItemToDrawQueue(int x, int y, int id);
void ShowItemNameLabels(CelOutputBuffer out);

} // namespace devilution
