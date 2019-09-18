#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "TestAP";
const char* PASSWORD = "";
const char* ENDPOINT = "https://us-central1-frid-vue.cloudfunctions.net/updateTemp";
const int INTERVAL = 5 * 60 * 1000;

const char* DEVICE_ID = "0";

/* Required: arduino-esp32
 * pinout:
 *   ESP32 GPIO4 -             LM61ciz center
 *   ESP32 3.3V  - IM920c Vcc, LM61ciz Vcc
 *   ESP32 GND   - IM920c GND, LM61ciz GND
 */

void setup() {
  Serial.begin(115200);
  Serial.print("Attempting to connect");
  while(WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PASSWORD);
    Serial.print(".");
    delay(500);
  }
  Serial.println("connected: ");
  Serial.print(WiFi.localIP());
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
  float voltage = analogRead(4) * 3.3f / 4096.0f;
  float temprature = (voltage - 0.6) * 100.0f;

  HTTPClient http;
  http.begin(ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  auto body = buildBody(0, temperature);
  auto httpResponseCode = http.POST(body);
  http.end();

  Serial.print("http status code: ");
  Serial.println(httpResponseCode);

  delay(INTERVAL);
}
