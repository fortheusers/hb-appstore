#include "Keyboard.hpp"

Keyboard::Keyboard()
{

}

void Keyboard::render(Element* parent)
{
  SDL_Rect dimens = { 880, 0, 400, 150 };

  this->window = parent->window;
  this->renderer = parent->renderer;

	SDL_SetRenderDrawColor(parent->renderer, 0xf9, 0xf9, 0xf9, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);

  // draw a qwerty keyboard
  std::string row1 = "Q W E R T Y U I O P";
  std::string row2 =  "A S D F G H J K L";
  std::string row3 =   "Z X C V B N M";

  std::vector<std::string> rows = std::vector<std::string>();
  rows.push_back(row1);
  rows.push_back(row2);
  rows.push_back(row3);
  SDL_Color gray = { 0x52, 0x52, 0x52, 0xff };

  for (int x=0; x<rows.size(); x++)
  {
    TextElement* rowText = new TextElement(rows[x].c_str(), 30, &gray, true);
    rowText->position(910+x*22, 14+x*33);
    this->elements.push_back(rowText);
  }

  super::render(this);
}
