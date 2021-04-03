/**
 * @file qol.h
 *
 * Quality of life features
 */
#ifndef __QOL_H__
#define __QOL_H__

#include "engine.h"

DEVILUTION_BEGIN_NAMESPACE

void FreeQol();
void InitQol();
void DrawMonsterHealthBar(CelOutputBuffer out);
void DrawXPBar(CelOutputBuffer out);
void AutoGoldPickup(int pnum);

extern BYTE ItemAnimLs[];
extern BYTE *itemanims[ITEMTYPES];

void AltPressed(bool pressed);
bool IsGeneratingLabels();
void UpdateLabelOffsets(CelOutputBuffer out, BYTE *dst, int width);
void GenerateLabelOffsets(CelOutputBuffer out);
void AddItemToDrawQueue(int x, int y, int id);
void HighlightItemsNameOnMap(CelOutputBuffer out);

DEVILUTION_END_NAMESPACE

#endif /* __QOL_H__ */
