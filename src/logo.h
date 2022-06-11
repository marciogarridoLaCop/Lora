#include <heltec.h>
#include <lacop.h>

void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 0, uff_lacop_width, uff_lacop_height, (const unsigned char *)uff_lacop_bits);
  Heltec.display->display();
}