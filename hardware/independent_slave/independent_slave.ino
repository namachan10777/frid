#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "TestAP";
const char* PASSWORD = "weak_pass";
const char* ENDPOINT = "https://us-central1-frid-vue.cloudfunctions.net/updateTemp";
const int INTERVAL = 5 * 60 * 1000;
const int ADC_INPUT = 34;

const char* DEVICE_ID = "0";

/* Required: arduino-esp32
 * pinout:
 *   ESP32 GPIO4 -             LM61ciz center
 *   ESP32 3.3V  - IM920c Vcc, LM61ciz Vcc
 *   ESP32 GND   - IM920c GND, LM61ciz GND
 */

void setup() {
  WiFi.disconnect(true);
  delay(1000);
  Serial.begin(115200);
  Serial.print("Attempting to connect");
  while(WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PASSWORD);
    Serial.print(".");
    delay(500);
  }
  Serial.println("connected: ");
  Serial.print(WiFi.localIP());
  pinMode(INPUT, ADC_INPUT);
}

String buildBody(int id, float temp) {
  auto json = String("{\"id\":\"");
  json.concat(String(id));
  json.concat(String("\", \"temperature\":"));
  json.concat(String(temp));
  json.concat(String("}"));
  return json;
}

void loop() {
  Serial.println(analogRead(ADC_INPUT));
  float voltage = analogRead(ADC_INPUT) * 3.3f / 4096.0f;
  float temperature = (voltage - 0.6) * 100.0f;

  HTTPClient http;
  http.begin(ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  auto body = buildBody(0, temperature);
  auto httpResponseCode = http.POST(body);
  http.end();

  /*
   * Try this method in below issue, if Arduino failed to sene POST header.
   * https://github.com/espressif/arduino-esp32/issues/2670#issuecomment-483289542
   */
  Serial.print("temp: ");
  Serial.println(voltage);
  Serial.println(temperature);
  Serial.print("http status code: ");
  if (httpResponseCode < 0) {
    Serial.println(http.errorToString(httpResponseCode));
  }
  else {
    Serial.println(httpResponseCode);
  }
  for (int i = 0; i * 10 < INTERVAL; ++i) {
    if (digitalRead(5) == LOW) {
      http.begin("https://us-central1-frid-vue.cloudfunctions.net/disableFood");
      Serial.println("disable");
      http.addHeader("Content-Type", "application/json");
      auto json = String("{\"id\":\"0\"}");
      http.POST(json);
      http.end();
    }
    delay(10);
  }
}
