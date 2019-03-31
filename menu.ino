// Color definitions
//#define ILI9341_BLACK       0x0000  ///<   0,   0,   0
//#define ILI9341_NAVY        0x000F  ///<   0,   0, 123
//#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
//#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
//#define ILI9341_MAROON      0x7800  ///< 123,   0,   0
//#define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
//#define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
//#define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
//#define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
//#define ILI9341_BLUE        0x001F  ///<   0,   0, 255
//#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
//#define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
//#define ILI9341_RED         0xF800  ///< 255,   0,   0
//#define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
//#define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
//#define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
//#define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
//#define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
//#define ILI9341_PINK        0xFC18  ///< 255, 130, 198


void drawHomeScreen()
{

  //screen is 240 x 320
  //graphics primitives documentation  https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives

  //top left corner is (0,0)
  //void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  //void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  //void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
  //void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
  //X, Y pair for the top-left corner of the rectangle, a width and height (in pixels), and a color

  tft.fillScreen(ILI9341_BLACK);
  tft.drawRoundRect(0, 0, 239, 319, 6, ILI9341_WHITE);     //Screen border
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(9, 25);
  tft.println("Ark Vending Machine Demo");
  tft.setTextColor(ArkRed);
  tft.setCursor(33, 50);
  tft.println("Select a Candy");

  //create buttons
  tft.fillRoundRect(30, 70, 130, 40, 7, ArkRed);     //M&Ms
  tft.drawRoundRect(30, 70, 130, 40, 7, ILI9341_WHITE);

  tft.fillRoundRect(30, 120, 130, 40, 7, ArkRed);    //Smarties
  tft.drawRoundRect(30, 120, 130, 40, 7, ILI9341_WHITE);

  tft.fillRoundRect(30, 170, 130, 40, 7, ArkRed);    //Skittles
  tft.drawRoundRect(30, 170, 130, 40, 7, ILI9341_WHITE);

  //create button text
  tft.setTextColor(ILI9341_BLACK);

  tft.setCursor(70, 95);
  tft.print("M&Ms");

  tft.setCursor(60, 145);
  tft.print("Smarties");

  tft.setCursor(67, 195);
  tft.print("Skittles");

  Serial.println("finished drawing home screen");
}



void handleTouchscreen()
{
  if (ts.touched()) {
    TS_Point p = ts.getPoint(); //read raw touchscreen data from buffer
    //  delay(5);
    //p = ts.getPoint();

    //    Serial.print("X = "); Serial.print(p.x);
    //   Serial.print("\tY = "); Serial.print(p.y);
    //    Serial.print("\tPressure = "); Serial.println(p.z);


    // Scale from ~0->4000 to tft.width using the calibration #'s
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 239);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 319);

    p.x = constrain(p.x, 0, 239);
    p.y = constrain(p.y, 0, 319);

    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);

    //NOTE: We are not currently checking pressure data for points

    //check for M&Ms button being pressed
    if (p.x > 30 && p.x < (30 + 130) && p.y > 70 && p.y < (70 + 40))   {
      tft.fillRoundRect(30, 70, 130, 40, 7, ArkLightRed);     //M&Ms
      tft.setCursor(70, 95);
      tft.print("M&Ms");
      delay(50);
      while (ts.touched()) {}       //wait until screen is no longer being touched
      while (!ts.bufferEmpty()) {
        p = ts.getPoint();          //empty buffer
      }
      tft.fillRoundRect(30, 70, 130, 40, 7, ArkRed);     //M&Ms
      tft.drawRoundRect(30, 70, 130, 40, 7, ILI9341_WHITE);
      tft.setTextColor(ILI9341_BLACK);
      tft.setCursor(70, 95);
      tft.print("M&Ms");
      Serial.println("Selected M&Ms");
    }

    //check for smarties button being pressed
    else if (p.x > 30 && p.x < (30 + 130) && p.y > 120 && p.y < (120 + 40))   {
      tft.fillRoundRect(30, 120, 130, 40, 7, ArkLightRed);
      tft.setCursor(60, 145);
      tft.print("Smarties");
      delay(50);
      while (ts.touched()) {}       //wait until screen is no longer being touched
      while (!ts.bufferEmpty()) {
        p = ts.getPoint();          //empty buffer
      }
      tft.fillRoundRect(30, 120, 130, 40, 7, ArkRed);
      tft.drawRoundRect(30, 120, 130, 40, 7, ILI9341_WHITE);
      tft.setTextColor(ILI9341_BLACK);
      tft.setCursor(70, 95);
      tft.setCursor(60, 145);
      tft.print("Smarties");
      Serial.println("Selected Smarties");
    }

    //check for skittles button being pressed
    else if (p.x > 30 && p.x < (30 + 130) && p.y > 170 && p.y < (170 + 40))   {
      tft.fillRoundRect(30, 170, 130, 40, 7, ArkLightRed);
      tft.setCursor(67, 195);
      tft.print("Skittles");
      delay(50);
      while (ts.touched()) {}       //wait until screen is no longer being touched
      while (!ts.bufferEmpty()) {
        p = ts.getPoint();          //empty buffer
      }
      tft.fillRoundRect(30, 170, 130, 40, 7, ArkRed);
      tft.drawRoundRect(30, 170, 130, 40, 7, ILI9341_WHITE);
      tft.setTextColor(ILI9341_BLACK);
      tft.setCursor(67, 195);
      tft.print("Skittles");
      Serial.println("Selected Skittles");
    }
  }
  delay(10);
}
