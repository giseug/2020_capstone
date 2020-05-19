#include <SoftwareSerial.h>
#include  <DHT.h>

//토양수분
int soil = 0; //analog A0
int value_soil = 0;

//워터펌프 & 모터 드라이버
#define BIA 9 //digital 9
#define BIB 10 // digital 10
//int waterpump = 200;

//온습도센서
#define DHTPIN 5 //digital 5
#define DHTTYPE DHT22
DHT dht(DHTPIN,DHTTYPE);
float temp; //temperature
float hum; //humidity

//fan
#define fan 4; //digital 4

//조도센서
int CDS=1;// analog A1
int value_illu = 0;

//수위센서
int water_level = 2; //analog A2
int value_wat = 0;


//블루투스
SoftwareSerial BTSerial(2,3); //(RX,TX)
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600);

  pinMode(fan,OUTPUT);
  pinMode(BIA,OUTPUT);
  pinMode(BIB,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
   * 수동/자동 구분
   */
  //온습도
  temp = dht.readTemperature();
  hum=dht.readHumidity();
  //computer out
  Serial.print("temperature : ");
  Serial.println(temp);
  Serial.print("humidity : ");
  Serial.println(hum);
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
//  delay(1000);
  
  //조도값 전달
  value_illu = analogRead(CDS);
  value_illu = map(value_illu, 400,1000,0,100);
  Serial.print("illuminance : ");
  Serial.println(value_illu);
  BTSerial.write(byte(value_illu));
  /*
   * 조도에 따라 led사용
   */
//  delay(1000);

  // 토양수분
  value_soil = analogRead(soil);
  Serial.print("soil : ");
  Serial.println(value_soil);
  BTSerial.print(value_soil);
  
  //워터펌프 작동
   if(value_soil<500){ //실제 측정값으로 값 넣기
      //5초동안만 작동 (다만, 5초후에도 부족하면 다시 5초동안 작동)
      analogWrite(BIA,LOW);
      analogWrite(BIB,HIGH);
      delay(5000);
      analogWrite(BIB,LOW);

      /* 물이 부족한 상태면 계속 물을 주는 방식.
      analogWrite(BIA,LOW);
      analogWrite(BIB,HIGH);
   }
   else{
    analogWrite(BIA,LOW);
    analogWrite(BIB,LOW);
   }
   delay(2000);
   */
  }

  //수위센서
  value_wat = analogRead(water_level);
  Serial.print("water level : ");
  Serial.println(value_wat);
  BTSerial.println(value_wat);
  if(value_wat<500){ //수위 값 조정 필요
    /*
     *수위 조정시 앱에 들어갈 내용
     *BTSerial.println("water_level is low);
     */
  }
}
