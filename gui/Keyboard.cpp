#include "Keyboard.hpp"
#include "AppList.hpp"

Keyboard::Keyboard(AppList* appList)
{
  this->rows = std::vector<std::string>();
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

  TextElement* delButton = new TextElement("DEL", 16, &gray, false);
  delButton->position(900, 90);
  this->elements.push_back(delButton);

  TextElement* spaceButton = new TextElement("SPACE", 16, &gray, false);
  spaceButton->position(1210, 90);
  this->elements.push_back(spaceButton);

  this->appList = appList;
}

void Keyboard::render(Element* parent)
{
  SDL_Rect dimens = { 880, 0, 400, 135 };

  this->window = parent->window;
  this->renderer = parent->renderer;

	SDL_SetRenderDrawColor(parent->renderer, 0xf9, 0xf9, 0xf9, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);

  for (int y=0; y<3; y++)
    for (int x=0; x<10 - y - (y==2); x++)
    {
      SDL_Rect dimens2 = {903 + x*36 + y*22, 17 + y*33, 32, 32};
      // SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
      SDL_SetRenderDrawColor(parent->renderer, 0xf4, 0xf4, 0xf4, 0xff);
      SDL_RenderFillRect(parent->renderer, &dimens2);
    }

  // SDL_Rect dimens3 = {893, 85, 42, 32};
  // SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
  // SDL_RenderFillRect(parent->renderer, &dimens3);
  //
  // SDL_Rect dimens4 = {1206, 85, 55, 32};
  // SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
  // SDL_RenderFillRect(parent->renderer, &dimens4);

  super::render(this);

}

bool Keyboard::process(InputEvents* event)
{
  if (event->isTouchUp() && event->touchIn(880, 0, 400, 135))
  {
    for (int y=0; y<3; y++)
      for (int x=0; x<10 - y - (y==2); x++)
        if (event->touchIn(903 + x*36 + y*22, 17 + y*33, 32, 32))
          appList->sidebar->searchQuery += std::tolower(rows[y][x*2]);

    if (event->touchIn(893, 85, 42, 32))
      appList->sidebar->searchQuery.pop_back();

    if (event->touchIn(1206, 85, 55, 32))
      appList->sidebar->searchQuery += " ";

    // update search results
    this->appList->y = 0;
    this->appList->update();
  }

  return true;
}
