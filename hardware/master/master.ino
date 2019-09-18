#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "TestAP";
const char* PASSWORD = "";
const char* ENDPOINT = "https://us-central1-frid-vue.cloudfunctions.net/updateTemp";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
  WiFi.begin(SSID, PASSWORD);
  	Serial.print(".");
	delay(500);
  }

  Serial.println(" done!");
}

String buildBody(int id, int temp) {
  auto json = String("{\"id\":\"");
  json.concat(String(id));
  json.concat(String("\", \"temperature\":"));
  json.concat(String(temp));
  json.concat(String("}"));
  return json;
}

void loop() {
  auto temp = random(0, 100);
  HTTPClient http;
  http.begin(ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  auto body = buildBody(0, temp);
  auto httpResponseCode = http.POST(body);
  http.end();
  Serial.print("http status code");
  Serial.println(httpResponseCode);
  delay(1000);
}
