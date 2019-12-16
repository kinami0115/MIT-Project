
#include <SPI.h>//NFC카드
#include <MFRC522.h> //NFC카드
#include <DHT11.h> //온습도
#include <Wire.h> //I2C 
#include <Servo.h>    // 서보 라이브러리를 지정
#define SLAVE 20 //슬레이브 아두이노 주소
#define SS_PIN 10
#define RST_PIN 9
#define servoPin3 2 // 서보 모터핀을 지정
#define servoPin2 8// 서보 모터핀을 지정
#define servoPin1 7 // 서보 모터핀을 지정
Servo servo1,servo2,servo3;      // 서보 라이브러리 변수를 선언
MFRC522 rfid(SS_PIN, RST_PIN); //rfid 객체 생성
int pin=4; //온습도 핀번호
int buzzer = 3; //부저 핀번호
int angle =0; //1번 서보모터 각도
int doorangle=0; //2번 서모보터 각도
int distance; //초음파 거리
int trig =5;// trig 핀번호
int echo =6; //echo 빈번호
int y= analogRead(2); //불꽃 감지 센서 A2 핀 설정
DHT11 dht11(pin);
int light = 0; //빛 조절
int fire =0; //fire
int window =0; //window
void setup() 
{
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT); 
  pinMode(echo, INPUT);
  servo1.attach(servoPin1);  // 서보모터 핀을 설정한다
  servo2.attach(servoPin2);  // 서보모터 핀을 설정한다
  servo3.attach(servoPin3);  // 서보모터 핀을 설정한다
  servo2.write(125);        // 서보모터2 90도 위치로 초기화
  servo3.write(90);       // 서보모터3 0도 위치로 초기화
  Serial.begin(9600);   // 시리얼 통신 시작
  SPI.begin();      // SPI 통신 시작
  Wire.begin(); // Wire 라이브러리 초기화
  rfid.PCD_Init();   // rfid(MFRC522) 초기화
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  digitalWrite(buzzer,HIGH);//부저 HIGH

}
void loop(){
  int err; //온습도 에러 
  float temp, humi; //온도와 습도 값
  
  i2c_communication(); // 슬레이브로 데이터 요구 및 수신 데이터 처리
  for (angle = 20; angle < 180; angle++){
    servo1.write(angle);
    delay(15);
    //Serial.println(y);
    if(angle%23==0){ //lookdoor 시작
      lookdoor();
    }
    if(angle%17==0){ //card 시작
      card();
    }
    if(angle%11==0){ //dis 시작
    dis();
      if(distance<=5&&distance>0){//초음파 거리가 5cm 이하 일때
        Serial.println(1);
       // Serial.println(distance);
        servo1.detach();//서보모터 정지
        delay(5000);
        servo1.attach(servoPin1);//서보모터 재시작
      }//second if
    }//first if
  }for(angle = 179; angle >=20; angle--){
    servo1.write(angle);
    delay(15);
    if(angle%23==0){
      lookdoor();
    }
    if(angle%30==0){
      card();
    }
    if(angle%11==0){
      dis();
      if(distance<=5&&distance>0){
        Serial.println(1);
        servo1.detach();
        delay(5000);
        servo1.attach(servoPin1);
      }//second if
    }//first if
  }//for(CCTV)
  if((err=dht11.read(humi, temp))==0){
    
    if(temp>25&&window==0){
      Serial.println(3);
      for(int i=90; i>=0;i--){
       servo3.write(i); 
       window=1;
      }
    }else if(temp<22&&window==1){
      Serial.println(4);
      for(int i=0; i<=90;i++){
      servo3.write(i); 
      window=0;
      }
    }
  }else{
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err);
    Serial.println();    
  }
  if(y<800){//불꽃이 감지 되면 작동
    digitalWrite(buzzer,LOW); //부저 on
    if(fire==0){
      Serial.println(5);
      
      fire=1;
    }
  }else{
    digitalWrite(buzzer,HIGH); //부저 off
    fire=0;
  }
  }

void card(){//카드
  // 새카드 접촉이 있을 때만 다음 단계로 넘어감
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // 카드 읽힘이 제대로 되면 다음으로 넘어감
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  // UID 값을 16진 값으로 읽고 저장한 후 시리얼 모니터로 표시함
  //Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == "29 19 92 A3")// 엑세스 승인 하고자하는 UID 기록
  { 
  // 인증이 되면 Green LED와 함께 서보모터를 작동시킨다.
    Serial.println("Authorized access");
    Serial.println(2);
    for(doorangle=125; doorangle>=50;doorangle--){
      servo2.write(doorangle);
      delay(5);
    }
    delay(5000); // 시간지연 후 문을 닫는다  
   for(doorangle=50; doorangle<=125;doorangle++){
      servo2.write(doorangle);
      delay(5);
    }     
  }
  else{    
    Serial.println(" Access denied"); // 그외 UID는 승인 거부 표시
  }

}


void dis(){ //초음파센서
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  distance = pulseIn(echo,HIGH)/58; //거리 계산 저장
}  
void lookdoor(){//버튼
  int i;
  i=analogRead(3);//A3 
  if(i>1000){//버튼 값이 들어오면 작동
    Serial.println(1);
    servo1.write(90);
    delay(3000);
  }
}
void i2c_communication() {
  Wire.requestFrom(SLAVE, 1); // 1 바이트 크기의 데이터 요청
  char c = Wire.read(); // 수신 데이터 읽기
  if(c =='6'&& light ==0){
  Serial.println(c); // 수신 데이터 출력
  light =1;
  }else if(c=='1'&&light==1){
  light=0;
  }
}
