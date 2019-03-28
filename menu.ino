
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
  tft.drawRoundRect(0, 0, 240, 320, 6, WHITE);     //Screen border
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 20);
  tft.println("Ark Vending Machine Demo");
  tft.println("Select a Candy");  

//create buttons
  tft.fillRoundRect(60, 80, 200, 40, 7, RED);     //M&Ms
  tft.drawRoundRect(60, 80, 200, 40, 7, WHITE); 
  
  tft.fillRoundRect(60, 130, 200, 40, 7, RED);    //Smarties
  tft.drawRoundRect(60, 130, 200, 40, 7, WHITE);
  
  tft.fillRoundRect(60, 180, 200, 40, 7, RED);    //Skittles
  tft.drawRoundRect(60, 180, 200, 40, 7, WHITE);  

//create button text
  tft.setTextColor(BLACK);

  tft.setCursor(100, 100);
  tft.print("M&Ms");
 
  tft.setCursor(80, 150);
  tft.print("Smarties");

  tft.setCursor(80, 200);
  tft.print("Skittles");


}
