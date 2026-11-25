#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

SSD1306AsciiWire oled;
// Use &SH1106_128x64 if your module is specified as SH1106.
const DevType* const profile = &Adafruit128x64;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setWireTimeout(5000, true);
  delay(200);
  Serial.println(F("OLED test at 100 kHz"));
  for (byte address = 0x3C; address <= 0x3D; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    Serial.print(F("0x"));
    Serial.print(address, HEX);
    Serial.print(F(" result "));
    Serial.println(error);
    if (error != 0) continue;

    oled.begin(profile, address);
    oled.setFont(System5x7);
    oled.clear();
    oled.set2X();
    oled.println(F("VisionX"));
    oled.println(F("OLED OK"));
    Serial.println(F("Text sent. Check the screen."));
    return;
  }
  Serial.println(F("No OLED response at 0x3C or 0x3D."));
}

void loop() {}
