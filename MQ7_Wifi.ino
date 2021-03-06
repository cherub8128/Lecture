#include <Arduino.h>
/****************************와이파이 라이브러리************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <MQUnifiedsensor.h>
/************************Hardware Related Macros************************************/
#define         Board                   ("Arduino D1 R1")
#define         Pin                     (A0)  //Analog input 0 of your arduino
/***********************Software Related Macros*************************************/
#define         Type                    ("MQ-7") //MQ7
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10)
#define         RatioMQ7CleanAir        (27.5) //RS / R0 = 27.5 ppm 
/*****************************Globals***********************************************/

MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type); //MQ Sensor

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

    /*****************************  MQ 센서 설정 ********************************************/
    //Set math model to calculate the PPM concentration and the value of constants
    MQ7.setRegressionMethod(1); //_PPM =  a*ratio^b
    MQ7.setA(99.042); MQ7.setB(-1.518); // Configurate the ecuation values to get CO concentration
    /*
    Exponential regression:
    GAS     | a      | b
    H2      | 69.014  | -1.374
    LPG     | 700000000 | -7.703
    CH4     | 60000000000000 | -10.54
    CO      | 99.042 | -1.518
    Alcohol | 40000000000000000 | -12.35
    */
    MQ7.init();
    Serial.print("Calibrating please wait.");
    float calcR0 = 0;
    for(int i = 1; i<=10; i ++)
    {
        MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
        calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
        Serial.print(".");
    }
    MQ7.setR0(calcR0/10);
    Serial.println("  done!.");
    if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
    if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
    MQ7.serialDebug(true);

    /********************************** WiFi 접속 ********************************************/
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

    MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
    float PPM = MQ7.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    MQ7.serialDebug(); // Will print the table on the serial port
    
    // 인터넷에 값 올리기
    if (https.begin(*client, url+String(MQ7._PPM,5))) {
        int httpCode = https.GET();
        https.end();
       
        // 값 업로드 성공하면 5초(5000ms) 간 기다리기
        delay(5000); 
    }
}