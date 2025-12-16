#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // hoặc 0x3F nếu 0x27 không chạy

void setup() {
  Wire.begin(D2, D1);  // SDA, SCL của ESP8266
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("I LOVE YOU");
  lcd.setCursor(0, 1);
  lcd.print("<3 ESP8266 <3");
}

void loop() {}
