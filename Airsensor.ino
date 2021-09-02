/*
 MOSI - pin 11
 MISO - pin 12
 CLK - pin 13
 CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 CO2센서 - pin A0 측정범위
 CH4센서 - pin A1 측정범위 300pH~10000pH
*/

#include <SPI.h>
#include <SD.h>
#include <MQUnifiedsensor.h>

/************************Hardware Related Macros************************************/
#define MG_PIN      (A0)    //아날로그 값을 입력받을 핀을 정의 
#define BOOL_PIN    (2)     //디지털값을 입력받을 핀을 정의
#define DC_GAIN     (8.5)   //증폭회로의 전압이득 정의(변경 X)

/***********************Software Related Macros************************************/
#define READ_SAMPLE_INTERVAL    (50)    //샘플 값 추출 간격
#define READ_SAMPLE_TIMES       (5)     //추출할 샘플값의 갯수 
                                        //추출할 샘플값들의 평균값이 측정값입니다.
/**********************Application Related Macros**********************************/

//These two values differ from sensor to sensor. user should derermine this value.
#define ZERO_POINT_VOLTAGE  (0.220) //이산화 탄소가 400ppm일때의 전압값 (수정 X)
#define REACTION_VOLTGAE    (0.030) //이산화 탄소가 1000ppm일때의 전압값(수정 X)
/*****************************Globals***********************************************/
float CO2Curve[3] = {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};
                                                     //two points are taken from the curve.
                                                     //with these two points, a line is formed which is
                                                     //"approximately equivalent" to the original curve.
                                                     //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280)
                                                     //slope = ( reaction voltage ) / (log400 –log1000)

/************************Hardware Related Macros************************************/
#define         Board                   ("Arduino UNO")
#define         Pin                     (A1)  //Analog input 4 of your arduino
/***********************Software Related Macros************************************/
#define         Type                    ("MQ-4") //MQ4
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (4.4) //RS / R0 = 60 ppm 
/*****************************Globals***********************************************/
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);


File myFile;
// 아날로그핀 A0, A1에 꽂힌 센서값 저장용 변수
float val_A1 = 0;
// 측정시간 기록
int minute = 0;
int hour = 0;
unsigned long day = 0;

void setup() {
    // 시리얼 통신 열기:
    Serial.begin(9600);
    pinMode(BOOL_PIN, INPUT);                        //set pin to input
    digitalWrite(BOOL_PIN, HIGH);                    //turn on pullup resistors

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

    MQ4.setRegressionMethod(1); //_PPM =  a*ratio^b
    MQ4.setA(1012.7); MQ4.setB(-2.786); // Configurate the ecuation values to get CH4 concentration
    MQ4.setR0(3.86018237); // Value getted on calibration
}

void loop() {
    int percentage;
    float volts;

    volts = MGRead(MG_PIN);
    Serial.println(volts) //깨끗한 공기(이산화탄소가 없는 공기)에서 측정 후 8.5로 나누어 ZERO_POINT_VOLTAGE로 둔다.
    percentage = MGGetPercentage(volts,CO2Curve);
    MQ4.init();
    MQ4.update();
    float ppmCH4 = MQ4.readSensor();

    myFile.print(day);
    myFile.print("일째");
    myFile.print(hour);
    myFile.print("시");
    myFile.print(minute);
    myFile.print("분,");
    myFile.print("CO2 농도,");
    if (percentage == -1) myFile.print( "<400" );
    else myFile.print(percentage);
    myFile.print("ppm,");
    myFile.print("CH4 농도,");
    myFile.print(ppmCH4);
    myFile.println("ppm");

    // 시리얼 화면에 읽은 값을 출력한다.
    Serial.print(day);
    Serial.print("일째");
    Serial.print(hour);
    Serial.print("시");
    Serial.print(minute);
    Serial.print("분,");
    Serial.print("CO2 농도,");
    if (percentage == -1) Serial.print( "<400" );
    else Serial.print(percentage);
    Serial.print("ppm,");
    Serial.print("CH4 농도,");
    Serial.print(ppmCH4);
    Serial.println("ppm");

    // 60*1000ms=60초 동안 딜레이를 준다.
    // 측정할때 들어간 딜레이를 빼야한다.
    delay(60000-READ_SAMPLE_INTERVAL*READ_SAMPLE_TIMES);

    if(++minute>60)
    {
        minute = 0;
        if(++hour>24)
        {
            hour = 0;
            day++;
        }
    }
}

/*****************************  MGRead *********************************************
Input:   mg_pin - analog channel
Output:  output of SEN-000007
Remarks: This function reads the output of SEN-000007
************************************************************************************/

float MGRead(int mg_pin)
{
    int i;
    float v=0;

    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;
}

/*****************************  MQGetPercentage **********************************
Input:   volts   - SEN-000007 output measured in volts
         pcurve  - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(MG-811 output) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/

int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE)
   {
      return -1;
   } 
   else 
   { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}