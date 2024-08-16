#include"spoor.h"

void status_bar_init(Graphic *graphic)
{
    CONFIG_STATUS_BAR_HEIGHT_SET(graphic->font.height * CONFIG_GRAPHIC_SCALE + CONFIG_STATUS_BAR_PADDING * 2);
}
