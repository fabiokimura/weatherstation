/**
 * Mapeia imagens em arquivo pra memoria
 */
#include "icons.h"

icons::icons(Adafruit_ILI9341 * tft) {
  _tft = tft;
}

void icons::drawBitmap(char *i, int x, int y) {
  boolean achou = false;
  String icon = String(i);
//  Serial.println(String(x)+","+String(y)+":["+icon+"]");
  if (icon == "/wi-dust.bmp") {
    _tft->drawBitmap(x, y, myBitmap1, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-sunset.bmp") {
    _tft->drawBitmap(x, y, myBitmap2, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-strong-wind.bmp") {
    _tft->drawBitmap(x, y, myBitmap3, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-raindrops.bmp") {
    achou = true;
    _tft->drawBitmap(x, y, myBitmap4, 40, 40,  ILI9341_BLUE);
  }
  if (icon == "/wifi1.bmp") {
    _tft->drawBitmap(x, y, myBitmap5, 24, 22,  ILI9341_RED);
    achou = true;
  }
  if (icon == "/wi-humidity.bmp") {
    _tft->drawBitmap(x, y, myBitmap6, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-barometer.bmp") {
    _tft->drawBitmap(x, y, myBitmap7, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-celsius.bmp") {
    _tft->drawBitmap(x, y, myBitmap8, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-fahrenheit.bmp") {
    _tft->drawBitmap(x, y, myBitmap9, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-direction-up-right.bmp") {
    _tft->drawBitmap(x, y, myBitmap10, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-direction-down-right.bmp") {
    _tft->drawBitmap(x, y, myBitmap11, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wi-sunrise.bmp") {
    _tft->drawBitmap(x, y, myBitmap12, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/wifi2.bmp") {
    _tft->drawBitmap(x, y, myBitmap13, 24, 18,  ILI9341_GREEN);
    achou = true;
  }
  if (icon == "/wi-snowflake-cold.bmp") {
    _tft->drawBitmap(x, y, myBitmap13, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  
  if (icon == "/weather/wi-cloud.bmp") {
    achou = true;
    _tft->drawBitmap(x, y, weatherBitmap1, 100, 100,  ILI9341_BLUE);
  }
  if (icon == "/weather/wi-rain.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap2, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-day-sunny.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap3, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-showers.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap4, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-fog.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap5, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-stars.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap6, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-thunderstorm.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap7, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-day-cloudy.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap8, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-sleet.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap9, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/wi-snow.bmp") {
    _tft->drawBitmap(x, y, weatherBitmap10, 100, 100,  ILI9341_BLUE);
    achou = true;
  }
  if (icon == "/weather/fan-on.bmp") {
    _tft->drawBitmap(x, y, fanOnBitmap, 40, 40,  ILI9341_BLUE);
    achou = true;
  }
  if( !achou) {
    _tft->setCursor(x,y);
    _tft->print(icon);
    Serial.println("nao achou:" + String(icon));
  }
  
}
