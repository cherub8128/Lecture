#include <Arduino.h>
#include <LiquidCrystal.h>//키패드쉴드 라이브러리

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //키패드 쉴드 LCD 관련 설정
char read_LCD_Buttons(); //키패드 쉴드 버튼 관련 함수
int minute = 60000; //1분

void setup() {

    //시리얼 통신 시작(보 레이트 - 1초에 몇 바이트 속도로 통신할 것인지)
    Serial.begin(115200);
    Serial.print("모니터링 시작");

    //키패드 쉴드 시작(사이즈 가로 16칸 세로 2칸)
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Start...");

}

void loop() {

    // A1에 꽂힌 센서 값 가져오기
    float A1_val = analogRead(1);

    // 온도
    float temperature = A1_val*0.1+15.9;

    // 시리얼 모니터에 온도 출력
    Serial.println(A1_val);
    Serial.println(temperature);

    // 키패드 쉴드에 온도 출력
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(String(temperature,1)+"*C");

}

// 키패드 쉴드 함수 아날로그 A0의 값에서 버튼 신호를 받는다.
// 버튼이 눌리면 위:u, 아래:d, 왼쪽:l, 오른쪽:r, 왼쪽:l, 선택:s 의 글자로 내보내준다.
// 현재는 센서랑 동시에 사용 불가
char read_LCD_Buttons() {
    int input = analogRead(0);
    if (input < 50) return 'r';
    else if (input < 175) return 'u';
    else if (input < 330) return 'd';
    else if (input < 520) return 'l';
    else if (input < 830) return 's';
    else return 'n';
}