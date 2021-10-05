#include <Arduino.h>
#include <LiquidCrystal.h>//키패드쉴드 라이브러리
//와이파이 라이브러리
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //키패드 쉴드 LCD 관련 설정
char read_LCD_Buttons(); //키패드 쉴드 버튼 관련 함수
ESP8266WiFiMulti WiFiMulti; //WiFi 관련 설정
int min = 60000;

//아래의 두 줄에서 따옴표 안쪽 부분만 수정해주세요.
String ssid = "와이파이 이름(알맞게 수정)";
String pw = "와이파이 비밀번호(알맞게 수정)";

void setup() {

    //시리얼 통신 시작(보 레이트 - 1초에 몇 바이트 속도로 통신할 것인지)
    Serial.begin(115200);

    //키패드 쉴드 시작(사이즈 가로 16칸 세로 2칸)
    lcd.begin(16, 2);

    //와이파이 접속 부분
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, pw);
    while ((WiFiMulti.run() != WL_CONNECTED))
    {
      Serial.print("No Connect");
      delay(1000);
    }
    Serial.println(WiFi.localIP());

}

void loop() {

    //인터넷 접속 준비
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    // 센서 값 가져오기
    float A1_val = analogRead(A1);

    // 온도
    float temperature = A1_val*0.1+15.9;

    // 시리얼 모니터에 온도 출력
    Serial.println(temperature);

    // 키패드 쉴드에 온도 출력
    lcd.setCursor(0,1);
    lcd.print(String(temperature)+"*C");

    // 인터넷에 값 올리기
    String url = "https://script.google.com/macros/s/AKfycbyG3K4sireewChPl6gqTgt0EQvU6Rw83tKHOtNN0qlKx7SqBFCRi31gml20PUwVDgxSug/exec?x=";
    if (https.begin(*client, url+String(val,1))) {
        int httpCode = https.GET();
        https.end();
        // 값 업로드 성공하면 1분 간 기다리기
        delay(1*min); 
    }
}

// 키패드 쉴드 함수 아날로그 A0의 값에서 버튼 신호를 받는다.
// 버튼이 눌리면 위:u, 아래:d, 왼쪽:l, 오른쪽:r, 왼쪽:l, 선택:s 의 글자로 내보내준다.
char read_LCD_Buttons() {
    int input = analogRead(0);
    if (input < 50) return 'r';
    else if (input < 175) return 'u';
    else if (input < 330) return 'd';
    else if (input < 520) return 'l';
    else if (input < 830) return 's';
    else return 'n';
}