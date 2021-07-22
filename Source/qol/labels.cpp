#include <string>
#include <unordered_set>
#include <vector>

#include "common.h"
#include "control.h"
#include "cursor.h"
#include "engine/point.hpp"
#include "engine/render/cel_render.hpp"
#include "gmenu.h"
#include "inv.h"
#include "labels.h"
#include "utils/language.h"

namespace devilution {

namespace {

enum class LabelType {
	LABEL_ITEM = 0,
	LABEL_OBJECT,
};

struct Label {
	int id, width;
	Point pos;
	std::string text;
	uint16_t color;
	LabelType type;
};

std::vector<Label> labelQueue;

bool altPressed = false;
bool isLabelHighlighted = false;
std::array<std::optional<int>, ITEMTYPES> itemLabelCenterOffsets;
bool invertHighlightToggle = false;

const int BorderX = 4;               // minimal horizontal space between labels
const int BorderY = 2;               // minimal vertical space between labels
const int MarginX = 2;               // horizontal margins between text and edges of the label
const int MarginY = 1;               // vertical margins between text and edges of the label
const int Height = 11 + MarginY * 2; // going above 13 scatters labels of items that are next to each other

} // namespace

void ToggleLabelHighlight()
{
	invertHighlightToggle = !invertHighlightToggle;
}

void ModifyCoords(int& x, int& y)
{
	y -= TILE_HEIGHT;
	if (!zoomflag) {
		x *= 2;
		y *= 2;
	}
}

int GetLabelWidth(const char * text)
{
	int width = GetLineWidth(text);
	width += MarginX * 2;
	return width;
}

void AltPressed(bool pressed)
{
	altPressed = pressed;
}

bool IsLabelHighlighted()
{
	return isLabelHighlighted;
}

bool IsHighlightingLabelsEnabled()
{
	return altPressed != invertHighlightToggle;
}

void AddItemToLabelQueue(int id, int x, int y)
{
	if (!IsHighlightingLabelsEnabled())
		return;
	ItemStruct *it = &Items[id];

	const char *textOnGround;
	if (it->_itype == ITYPE_GOLD) {
		std::sprintf(tempstr, _("%i gold"), it->_ivalue);
		textOnGround = tempstr;
	} else {
		textOnGround = it->_iIdentified ? it->_iIName : it->_iName;
	}

	int index = ItemCAnimTbl[it->_iCurs];
	if (!itemLabelCenterOffsets[index]) {
		std::pair<int, int> itemBounds = MeasureSolidHorizontalBounds(*it->AnimInfo.pCelSprite, it->AnimInfo.CurrentFrame);
		itemLabelCenterOffsets[index].emplace((itemBounds.first + itemBounds.second) / 2);
	}

	x += *itemLabelCenterOffsets[index];
	ModifyCoords(x, y);
	int nameWidth = GetLabelWidth(textOnGround);

	x -= nameWidth / 2;
	labelQueue.push_back(Label { id, nameWidth, { x, y }, textOnGround, it->getTextColor(), LabelType::LABEL_ITEM });
}

void AddObjectToLabelQueue(int id, int x, int y)
{
	if (!IsHighlightingLabelsEnabled())
		return;
	ObjectStruct *it = &Objects[id];

	if (!it->_oSelFlag)
		return;

	// TODO: remove this dirty hack after a proper way to get object name has been added
	char bkp[64];
	strcpy(bkp, infostr);
	uint16_t infoclr_bkp = infoclr, objcolor;
	GetObjectStr(id);
	objcolor = infoclr;
	infoclr = infoclr_bkp;
	char name[64];
	strcpy(name, infostr);
	strcpy(infostr, bkp);

	// center offset for objects seem to be somehow dynamic so it can't be stored and reused like the one for items
	CelSprite tmpCel(it->_oAnimData, it->_oAnimWidth);
	std::pair<int, int> objectBounds = MeasureSolidHorizontalBounds(tmpCel, it->_oAnimFrame);
	x += (objectBounds.first + objectBounds.second) / 2;
	ModifyCoords(x, y);
	int nameWidth = GetLabelWidth(name);
	x -= nameWidth / 2;;
	labelQueue.push_back(Label { id, nameWidth, { x, y }, name, objcolor, LabelType::LABEL_OBJECT });
}

bool IsMouseOverGameArea()
{
	if ((invflag || sbookflag) && MousePosition.x > RIGHT_PANEL && MousePosition.y <= SPANEL_HEIGHT)
		return false;
	if ((chrflag || QuestLogIsOpen) && MousePosition.x < SPANEL_WIDTH && MousePosition.y <= SPANEL_HEIGHT)
		return false;
	if (MousePosition.y >= PANEL_TOP && MousePosition.x >= PANEL_LEFT && MousePosition.x <= PANEL_LEFT + PANEL_WIDTH)
		return false;

	return true;
}

void FillRect(const Surface &out, int x, int y, int width, int height, Uint8 col)
{
	for (int j = 0; j < height; j++) {
		DrawHorizontalLine(out, { x, y + j }, width, col);
	}
}

void DrawLabels(const Surface &out)
{
	isLabelHighlighted = false;
	if ( leveltype == DTYPE_TOWN) invertHighlightToggle = false;
	else invertHighlightToggle = true;

	for (unsigned int i = 0; i < labelQueue.size(); ++i) {
		std::unordered_set<int> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				Label &a = labelQueue[i];
				Label &b = labelQueue[j];
				if (abs(b.pos.y - a.pos.y) < Height + BorderY) {
					int widthA = a.width + BorderX + MarginX * 2;
					int widthB = b.width + BorderX + MarginX * 2;
					int newpos = b.pos.x;
					if (b.pos.x >= a.pos.x && b.pos.x - a.pos.x < widthA) {
						newpos -= widthA;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = b.pos.x + widthB;
					} else if (b.pos.x < a.pos.x && a.pos.x - b.pos.x < widthB) {
						newpos += widthB;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = b.pos.x - widthA;
					} else
						continue;
					canShow = false;
					a.pos.x = newpos;
					backtrace.insert(newpos);
				}
			}
		} while (!canShow);
	}

	for (const Label &label : labelQueue) {
		Point pos = Items[label.id].position;
		int8_t* cursor = &pcursitem;

		if (label.type == LabelType::LABEL_OBJECT) {
			pos = Objects[label.id].position;
			cursor = &pcursobj;
		}

		if (MousePosition.x >= label.pos.x && MousePosition.x < label.pos.x + label.width && MousePosition.y >= label.pos.y - Height + MarginY && MousePosition.y < label.pos.y + MarginY) {
			if (!gmenu_is_active() && PauseMode == 0 && !MyPlayerIsDead && IsMouseOverGameArea()) {
				isLabelHighlighted = true;
				cursmx = pos.x;
				cursmy = pos.y;
				*cursor = label.id;
			}
		}
		if (*cursor == label.id)
			FillRect(out, label.pos.x, label.pos.y - Height + MarginY, label.width, Height, PAL8_BLUE + 6);
		else
			DrawHalfTransparentRectTo(out, label.pos.x, label.pos.y - Height + MarginY, label.width, Height);
		DrawString(out, label.text.c_str(), { label.pos.x + MarginX, label.pos.y, label.width, Height }, label.color);
	}
	labelQueue.clear();
}

} // namespace devilution
