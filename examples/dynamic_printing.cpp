#include "lcd.h"

LCD<hd44780::DataLength::FourBits> lcd(6, 8, {2, 3, 4, 5});

void setup() {
  lcd.initialize();
  lcd.print(F("HelloWorld:"));
}

void loop() {
  lcd.set_cursor_addr(0x40);
  lcd.print(F("1234"));
  delay(2000);
  lcd.set_cursor_addr(0x40);
  lcd.set_cursor_addr(F("5678"));
  delay(2000);
}