#include <SoftwareSerial.h>
#include  <DHT.h>

//토양수분
int soil = A0; //analog A0
#define soil_D 2 //digital 
int value_soil = 0;
int value_soil_D = 0;

//워터펌프 & 모터 드라이버
#define BIA 9 //digital 9
#define BIB 10 // digital 10
//int waterpump = 200;

//암막 모터
#define MTA 11 //digital 11
#define MTB 12 //digital 12

//온습도센서
#define DHTPIN 5 //digital 5
#define DHTTYPE DHT22
DHT dht(DHTPIN,DHTTYPE);
float temp; //temperature
float hum; //humidity

//fan
#define fan 4 //digital 4

//조도센서
int CDS = A1;// analog A1
int value_illu = 0;

int value_illu_min = 1023; //조도센서 최소값 1023으로 초기화
int value_illu_max = 0; //조도센서 최대값 0으로 초기화

//수위센서
int water_level = A2; //analog A2
int value_wat = 0;


//블루투스
SoftwareSerial BTSerial(2,3); //(RX,TX)
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  BTSerial.begin(9600);

  pinMode(soil_D, INPUT);
  pinMode(fan,OUTPUT);
  pinMode(BIA,OUTPUT);
  pinMode(BIB,OUTPUT);
  pinMode(MTA,OUTPUT);
  pinMode(MTB,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //BT연결확인
  if(BTSerial.available()){
    Serial.write(BTSerial.read()); //블루투스내용 시리얼 모니터에 출력
  }
  if(Serial.available()){
    BTSerial.write(Serial.read()); //시리얼 모니터 내용을 블루투스에 출력
  }
  
  int bt = BTSerial.read();
  /*
   * 수동/자동 구분
   */
  //온습도
  temp = dht.readTemperature();
  hum=dht.readHumidity();

  if(isnan(hum)||isnan(temp)){
    Serial.println("Faild temperature");
  }
  
  //computer out
  Serial.print("temperature : ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.print("humidity : ");
  Serial.print(hum);
  Serial.println(" %RH");
  Serial.println();
  //app out
  BTSerial.print(temp);
  BTSerial.print(hum);

  
  /*fan사용*/
  if(temp>27){
    digitalWrite(fan,HIGH);
  }
  else{
    digitalWrite(fan,LOW);
  }
  //어플에서 쿨러 on
  if( bt == '0'){
    digitalWrite(4,HIGH);
  }
  if( bt == '1'){
    digitalWrite(4,LOW);
  }
//  delay(1000);
  
  //조도값 전달
  value_illu = analogRead(CDS);
  value_illu = map(value_illu, value_illu_min, value_illu_max,0,255);//센서값을 0~255수로 변환
  value_illu = constrain(value_illu, 0,255); //0~255벗어날 경우 보정하여 저장
  Serial.print("illuminance : ");
  Serial.println(value_illu);
  BTSerial.write(byte(value_illu));

  //어두울때 -> led
  if(value_illu<=0){
    //led작동
  }
  //밝을때 -> 암막
  if(value_illu>=255){
    digitalWrite(MTA,HIGH);
    digitalWrite(MTB,HIGH);
    delay(5000);
  }

  /*모터속도 -> 속도조절로 암막커튼 펼치기.
  for(int duty = 0; duty<256;duty++){
    analogWrite(motorA,duty)
  }
  for(duty = 255; duty>=0;duty--){
    analogWrite(motorA,duty);
  }
  */

  // 토양수분
  value_soil = analogRead(soil);
  value_soil_D = digitalRead(soil_D);
  Serial.print("soil : ");
  Serial.println(value_soil);
  Serial.println();
  BTSerial.print(value_soil);
  
  //워터펌프 작동
   while(value_soil<500){ //실제 측정값으로 값 넣기
    //워터펌프 1개
    //5초동안만 작동 (다만, 5초후에도 부족하면 다시 5초동안 작동), 자동
    analogWrite(BIA,LOW);
    analogWrite(BIB,HIGH); 
  }
  digitalWrite(BIA,LOW);
  digitalWrite(BIB,LOW);
  
  //with BT, 수동
  if( bt == '5'){//waterpump on
    analogWrite(BIB,HIGH);
  }
  if( bt == '6'){//waterpump off
    analogWrite(BIB,LOW);
  }

  //수위센서
  value_wat = analogRead(water_level);
  Serial.print("water level : ");
  Serial.println(value_wat);
  BTSerial.println(value_wat);
  if(value_wat<500){ //수위 값 조정 필요
    BTSerial.println("low");
  }
}
