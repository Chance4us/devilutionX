/**
* @file itemlabels.h
*
* Adds Item Labels QoL feature
*/
#pragma once

namespace devilution {

void ToggleItemLabelHighlight(int i);
void AltPressed(bool pressed);
bool IsGeneratingItemLabels();
bool IsItemLabelHighlighted();
void UpdateItemLabelOffsets(const CelOutputBuffer &out, BYTE *dst, int width);
void GenerateItemLabelOffsets(const CelOutputBuffer &out);
void AddItemToLabelQueue(int x, int y, int id);
void DrawItemNameLabels(const CelOutputBuffer &out);

} // namespace devilution
