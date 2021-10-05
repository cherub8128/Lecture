#include <Arduino.h>

int minute = 60000; //ms로 표현한 1분 단위
float theta = 16.1; //그래프 보정 값

void setup() {

    //시리얼 통신 시작(보 레이트 - 1초에 몇 바이트 속도로 통신할 것인지)
    Serial.begin(115200);
    Serial.print("모니터링 시작");

}

void loop() {

    // A0에 꽂힌 센서 값 가져오기
    float A0_val = analogRead(0);

    // 측정값을 온도로 변환
    float temperature = A0_val/10+theta;

    // 시리얼 모니터에 온도 출력
    Serial.println(A0_val);
    Serial.println(temperature);
}