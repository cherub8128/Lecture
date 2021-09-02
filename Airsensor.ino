/*
 MOSI - pin 11
 MISO - pin 12
 CLK - pin 13
 CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 CO2센서 - pin A0
 CH4센서 - pin A1
*/

#include <SPI.h>
#include <SD.h>

File myFile;
// 아날로그핀 A0, A1에 꽂힌 센서값 저장용 변수
float val_A0 = 0;
float val_A1 = 0;
// 측정시간 기록
int minute = 0;
int hour = 0;
unsigned long day = 0;

void setup() {
    // 시리얼 통신 열기:
    Serial.begin(9600);

    // MicroSD카드가 연결되지 않은 경우
    if (!SD.begin(4))
    {
    Serial.println("MicroSD카드가 연결되지 않았습니다.");
    // 연결되지 않은 경우 동작을 아예 멈춤. 연결 후 다시 시도할 것.
    while (1);
    }
    Serial.println("연결 완료");

    // 쓰기 모드로 파일 열기. 한 번에 한 파일만 열 수 있고 다른 파일을 열려면 열려 있는 파일을 닫아야 함.
    myFile = SD.open("test.txt", FILE_WRITE);

    // 파일 열기가 성공하면:
    if (myFile)
    {
    Serial.print("test.txt 에 작성을 시작합니다...");
    } 
    else 
    {
    // 파일 열기가 실패한 경우:
    Serial.println("test.txt을 열 수 없습니다.");
    }

}

void loop() {
    // A0핀의 아날로그 값을 읽어서 val에 저장
    val_A0 = analogRead(A0);
    val_A1 = analogRead(A1);

    myFile.println("C02 농도, %d일째 %d시 %d분, %f", day, hour, minute, val_A0);
    myFile.println("CH4 농도, %d일째 %d시 %d분, %f", day, hour, minute, val_A1);

    // 시리얼 화면에 읽은 값을 출력한다.
    Serial.println("C02 농도, %d일째 %d시 %d분, %f", day, hour, minute, val_A0);
    Serial.println("CH4 농도, %d일째 %d시 %d분, %f", day, hour, minute, val_A1);

    // 60*1000ms=60초 동안 딜레이를 준다.
    delay(60000);

    if(++minute>60)
    {
        minute = 0
        if(++hour>24)
        {
            hour = 0
            day++;
        }
    }
}