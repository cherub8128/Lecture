#include <Arduino.h>
//와이파이 라이브러리
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti; //WiFi 관련 설정
int minute = 60000;
float theta = 16.1;

/***********!! 아래의 세 줄에서 따옴표 안쪽 부분을 수정해주세요. !!**************************/
char* ssid = "와이파이 이름";
char* pw = "와이파이 비번";
String url = "구글 스크립트 편집기 배포 웹 앱 URL";
/***********!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**************************/

void setup() {

    Serial.begin(115200);
    Serial.print("모니터링 시작");

    //와이파이 접속 부분
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, pw);
    while ((WiFiMulti.run() != WL_CONNECTED))
    {
      Serial.println("No Connect");
      delay(1000);
    }
    Serial.print(WiFi.localIP()); Serial.println("Connected");

}

void loop() {

    //인터넷 접속 준비
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    // A0에 꽂힌 센서 값 가져오기
    float A0_val = analogRead(0);

    // 온도
    float temperature = A0_val/10+theta;

    // 시리얼 모니터에 온도 출력
    Serial.println(A0_val);
    Serial.println(temperature);

    // 인터넷에 값 올리기
    if (https.begin(*client, url+String(temperature,1))) {
        int httpCode = https.GET();
        https.end();
       
        // 값 업로드 성공하면 1분 간 기다리기
        delay(1*minute); 
    }
}