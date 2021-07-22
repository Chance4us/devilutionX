/**
* @file labels.h
*
* Adds item labels QoL feature
*/
#pragma once

#include "engine.h"

namespace devilution {

void ToggleLabelHighlight();
void AltPressed(bool pressed);
bool IsLabelHighlighted();
bool IsHighlightingLabelsEnabled();
void AddItemToLabelQueue(int id, int x, int y);
void AddObjectToLabelQueue(int id, int x, int y);
void DrawLabels(const Surface &out);

} // namespace devilution
