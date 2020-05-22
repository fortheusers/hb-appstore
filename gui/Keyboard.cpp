#include "Keyboard.hpp"
#include "AppList.hpp"
#include "Feedback.hpp"

Keyboard::Keyboard(AppList* appList)
	: appList(appList)
{
	this->x = 372;
	this->y = 417;

	this->isAbsolute = true;

	// set touchmode based on applist if it's present
	if (appList)
		touchMode = appList->touchMode;

	curRow = index = -1;

	// position the keyboard based on this x and y
	updateSize();
}

void Keyboard::render(Element* parent)
{
	if (hidden)
		return;

	CST_Rect dimens = { this->x, this->y, this->width, this->height };

	this->window = parent->window;
	this->renderer = parent->renderer;

	CST_Color keyColor = { 0xf9, 0xf9, 0xf9, 0xFF };
	CST_SetDrawColor(parent->renderer, keyColor);
	CST_FillRect(parent->renderer, &dimens);

	CST_Color keyColor2 = { 0xf4, 0xf4, 0xf4, 0xff };
	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 10 - y - (y == 2); x++)
		{
			CST_Rect dimens2 = { this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth };
			CST_SetDrawColor(parent->renderer, keyColor2);
			CST_FillRect(parent->renderer, &dimens2);
		}

	// if there's a highlighted piece set, color it in
	if (curRow >= 0 || index >= 0)
	{
		CST_Rect dimens2 = { this->x + kXPad + index * kXOff + curRow * yYOff, this->y + kYPad + curRow * ySpacing, keyWidth, keyWidth };

		// if we're on DEL or SPACE, expand the dimens width of the highllighted button
		if (curRow == 2 && index < 0)
		{
			CST_Rect dimens3 = { this->x + dPos, this->y + dHeight, dWidth, textSize };
			dimens2 = dimens3;
		}
		if (curRow == 2 && index > 6)
		{
			CST_Rect dimens4 = { this->x + sPos, this->y + dHeight, sWidth, textSize };
			dimens2 = dimens4;
		}

		CST_Color selectedColor = { 0xad, 0xd8, 0xe6, 0x90 };
		// draw the currently selected tile if these index things are set
		if (touchMode)
		{
			CST_SetDrawColor(parent->renderer, selectedColor); // TODO: matches the DEEP_HIGHLIGHT color
			CST_FillRect(parent->renderer, &dimens2);
		}
		else
		{
			// border
			for (int z = 4; z >= 0; z--)
			{
				CST_Color highlight = { 0x66 - z * 10, 0x7c + z * 20, 0x89 + z * 10, 0xFF };
				CST_SetDrawColor(parent->renderer, highlight);
				dimens2.x--;
				dimens2.y--;
				dimens2.w += 2;
				dimens2.h += 2;
				CST_DrawRect(parent->renderer, &dimens2);
			}
		}
	}

	//   CST_Rect dimens3 = {this->x+dPos, this->y + dHeight, dWidth, textSize};
	//   CST_SetDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
	//   CST_FillRect(parent->renderer, &dimens3);
	//
	//   CST_Rect dimens4 = {this->x+sPos, this->y + dHeight, sWidth, textSize};
	//   CST_SetDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
	//   CST_FillRect(parent->renderer, &dimens4);

	super::render(parent);
}

bool Keyboard::process(InputEvents* event)
{
	// don't do anything if we're hidden, or there's a sidebar and it's active
	if (hidden || (appList && appList->sidebar->highlighted >= 0) || event->noop)
		return false;

	if (event->isTouchDown())
	{
		curRow = index = -1;
		touchMode = true;
	}

	if (event->isKeyDown())
		touchMode = false;

	bool ret = false;

	if (!touchMode)
	{
		if (curRow < 0 && index < 0)
		{
			// switched into keyboard, set to 0 and return
			curRow = index = 0;
			return true;
		}

		if (event->isKeyDown())
		{
			curRow += (event->held(DOWN_BUTTON) - event->held(UP_BUTTON));
			index += (event->held(RIGHT_BUTTON) - event->held(LEFT_BUTTON));

			if (curRow < 0) curRow = 0;
			if (index < (0 - curRow == 2)) index = -1 * (curRow == 2);
			if (curRow > 2) curRow = 2;
			if (index > 10 - curRow - 1) index = 10 - curRow - 1;

			if (event->held(A_BUTTON))
			{
				// on the last row, check for delete or space
				if (curRow == 2 && (index < 0 || index > 6))
				{
					if (index < 0) backspace();
					if (index > 6) space();
					inputChanged();
					return true;
				}
				type(curRow, index);
			}

			if (event->held(B_BUTTON))
			{
				if (textInput.empty() && appList)
				{
					appList->toggleKeyboard();
					return true;
				}
				backspace();
			}

			inputChanged();
			return true;
		}

		return false;
	}

	if (event->isTouchDown() && event->touchIn(this->x, this->y, width, height))
	{
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 10 - y - (y == 2); x++)
				if (event->touchIn(this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth))
				{
					ret |= true;
					curRow = y;
					index = x;
				}
		return true;
	}

	if (event->isTouchUp())
	{
		// only proceed if we've been touchdown'd
		// reset current row and info
		curRow = -1;
		index = -1;

		if (event->touchIn(this->x, this->y, width, height))
		{

			for (int y = 0; y < 3; y++)
				for (int x = 0; x < 10 - y - (y == 2); x++)
					if (event->touchIn(this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth))
					{
						ret |= true;
						type(y, x);
					}

			if (event->touchIn(this->x + dPos, this->y + dHeight, dWidth, textSize))
			{
				ret |= true;
				backspace();
			}

			if (event->touchIn(this->x + sPos, this->y + dHeight, sWidth, textSize))
			{
				ret |= true;
				space();
			}

			if (ret)
				inputChanged();

			return ret;
		}

		return false;
	}

	return false;
}

void Keyboard::updateSize()
{
	for (auto &i : this->elements)
		delete i;
	super::removeAll();

	this->width = 900;
	this->height = (304 / 900.0) * width;

	// set up lots of scaling variables based on the width/height

	this->keyWidth = (int)(0.08 * width);
	this->padding = keyWidth / 2.0;

	// these field variables are for displaying the QWERTY keys (touching and displaying)
	kXPad = (int)((23 / 400.0) * width);
	kXOff = (int)((36.5 / 400.0) * width);
	yYOff = (int)((22 / 400.0) * width);
	kYPad = (int)((17 / 135.0) * height);
	ySpacing = (int)((33 / 400.0) * width);

	// these local variables position only the text, and has nothing to do with the
	// touch. They should likely be based on the above field variables so those
	// can change quickly
	int kXPad = (int)((30 / 400.0) * width);
	int kXOff = (int)((22 / 400.0) * width);
	int kYPad = (int)((33 / 400.0) * width);
	int kYOff = (int)((33 / 400.0) * width);

	this->textSize = 0.9375 * keyWidth;

	// delete and space key dimensions
	dPos = (int)((13 / 400.0) * width);
	dHeight = (int)((85 / 135.0) * height);
	sPos = (int)((326 / 400.0) * width);

	dWidth = (int)(1.4125 * textSize);
	sWidth = (int)(1.91875 * textSize);

	CST_Color gray = { 0x52, 0x52, 0x52, 0xff };

	// go through and draw each of the three rows at the right position
	for (int x = 0; x < rowsCount; x++)
	{
		Element* rowText = new TextElement(rows[x].c_str(), textSize, &gray, true);
		rowText->position(kXPad + x * kXOff, kYPad + x * kYOff - (rowText->height / 2));
		super::append(rowText);
	}

	// these are local variables, similar to how the other ones are global events
	int dPos2 = (int)((20 / 400.0) * width);
	int dHeight2 = (int)((90 / 135.0) * height);
	int sPos2 = (int)((330 / 400.0) * width);

	int textSize2 = (int)((16 / 400.0) * width);

	Element* delButton = new TextElement("DEL", textSize2, &gray, false);
	delButton->position(dPos2, dHeight2);
	super::append(delButton);

	Element* spaceButton = new TextElement("SPACE", textSize2, &gray, false);
	spaceButton->position(sPos2, dHeight2);
	super::append(spaceButton);
}

void Keyboard::type(int y, int x)
{
	textInput.push_back(std::tolower(rows[y][x * 2]));
}

void Keyboard::backspace()
{
	if (!textInput.empty())
		textInput.pop_back();
}

void Keyboard::space()
{
	textInput.append(" ");
}

const std::string& Keyboard::getTextInput()
{
	return textInput;
}

void Keyboard::inputChanged()
{
	if (inputCallback)
		inputCallback();
}

Keyboard::~Keyboard()
{
	for (auto &i : this->elements)
		delete i;
	super::removeAll();
}
