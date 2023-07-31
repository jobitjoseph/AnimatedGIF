#include <SPI.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240
#define BUFFER_SIZE 320

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);  // Create a sprite object "spr" associated with the TFT_eSPI object "tft"

AnimatedGIF gif;
uint16_t usTemp[BUFFER_SIZE];

#include "test.h"
#define GIF_IMAGE Camara_gif 

void setup() {
  Serial.begin(115200);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  spr.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT); // Create sprite of screen size
  
  gif.begin(BIG_ENDIAN_PIXELS);
}

void loop()
{
  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    while (gif.playFrame(true, NULL))
    {
      spr.pushSprite(0, 0); // Push the sprite to screen after every frame
      yield();
    }
    gif.close();
  }
}

void GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette;
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth + pDraw->iX > DISPLAY_WIDTH)
    iWidth = DISPLAY_WIDTH - pDraw->iX;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;

  if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
    return;

  if (pDraw->ucDisposalMethod == 2)
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  s = pDraw->pPixels;
  if (pDraw->ucHasTransparency) 
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    pEnd = s + iWidth;
    x = 0;
    while (x < iWidth)
    {
      c = ucTransparent - 1;
      d = &usTemp[0];
      while (c != ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
        {
          s--;
        }
        else
        {
          *d++ = usPalette[c];
        }
      }
      if (d > &usTemp[0]) 
      {
        spr.pushImage(pDraw->iX + x, y, d - &usTemp[0], 1, usTemp); // Push the image to the sprite
        x += d - &usTemp[0];
      }
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          x++;
        else
          s--;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;
    for (x=0; x<iWidth; x++)
    {
      usTemp[x] = usPalette[*s++];
    }
    spr.pushImage(pDraw->iX, y, iWidth, 1, usTemp);  // Push the image to the sprite
  }
} 
