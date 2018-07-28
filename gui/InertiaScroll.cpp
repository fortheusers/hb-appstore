#include "InertiaScroll.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

void InertiaScroll::handle(Element* elem, InputEvents* event)
{
  if (event->isTouchDown())
  {
    // make sure that the mouse down's X coordinate is over the app list (not sidebar)
    if (event->xPos < elem->x)
      return;

    // saw mouse down so set it in our element object
    elem->dragging = true;
    elem->lastMouseY = event->yPos;
  }
  // drag event for scrolling up or down
  else if (event->isTouchDrag())
  {
    if (elem->dragging)
    {
      int distance = event->yPos - elem->lastMouseY;
      elem->y += distance;
      elem->lastMouseY = event->yPos;

      // use the last distance as the rubber band value
      elem->elasticCounter = distance;
    }
  }
  else if (event->isTouchUp())
  {
    // mouse up, no more mouse down (TODO: fire selected event here)
    elem->dragging = false;

    // if the scroll offset is less than the total number of apps
    // (put on the mouse up to make it "snap" when going out of bounds)
    // TODO: account for max number of apps too (prevent scrolling down forever)
    if (elem->y > 0)
      elem->y = 0;
  }

  // if mouse is up, and there's some elastic counter left, burn out remaining elastic value
  if (!elem->dragging && elem->elasticCounter != 0)
  {
    elem->y += elem->elasticCounter;

    int positivity = elem->elasticCounter/abs(elem->elasticCounter);
    elem->elasticCounter += 10 * (-1 * positivity);

    // when the oval and the elastic counter don't match in positivity, reset it to 0
    if (elem->elasticCounter != 0 && elem->elasticCounter/abs(elem->elasticCounter) != positivity)
      elem->elasticCounter = 0;

    // TODO: same problem as above todo, also extract into method?
    if (elem->y > 0)
      elem->y = 0;
  }
}
