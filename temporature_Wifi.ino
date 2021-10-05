#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const uint8_t fingerprint[20] = {0x11,0xB6,0x60,0xD8,0x58,0x65,0xE6,0x26,0xEC,0x67,0x61,0x3A,0x2A,0xF4,0x89,0xE4,0xA6,0xDE,0xB3,0x44};

ESP8266WiFiMulti WiFiMulti;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("KT_GiGA_2G_6F10", "3cx42kb027");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print("No Connect");
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  float val = analogRead(A1)*0.1+15.6;
  Serial.println(val);
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  client->setInsecure();
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();

  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  String url = "https://script.google.com/macros/s/AKfycbyG3K4sireewChPl6gqTgt0EQvU6Rw83tKHOtNN0qlKx7SqBFCRi31gml20PUwVDgxSug/exec?x=";
  if (https.begin(*client, url+String(val,1))) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
  }
  delay(1800000);
}