//  ↑

/* Required: arduino-esp32
 * pinout:
 *   ESP32 GPIO4 -             LM61ciz center
 *   ESP32 3.3V  - IM920c Vcc, LM61ciz Vcc
 *   ESP32 GND   - IM920c GND, LM61ciz GND
 */

void setup() {
  Serial.begin(19200);
}

void loop() {
  delay(100);
  float voltage = analogRead(4) * 3.3f / 4096.0f;
  float temprature = (voltage - 0.6) * 100.0f;
  Serial.print("TXDA ");
  // encoding
  int digitTemp = temprature * 1000;
  Serial.println(digitTemp, HEX);
}
