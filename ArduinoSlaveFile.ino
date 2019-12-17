#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 3 // 네오픽셀 연결 디지털 핀 번호 적기
#define NUM_LEDS 2 // 네오픽셀 소자 수, 1부터 시작. (3개 연결시, 3 작성)
#define SLAVE 20
byte mes = '6';
byte mes2= '1';
int cds;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  // Wire 라이브러리 초기화
  // 슬레이브로 참여하기 위해서는 주소를 지정해야 한다.
  Wire.begin(SLAVE);
  // 마스터의 데이터 전송 요구가 있을 때 처리할 함수 등록
  Wire.onRequest(sendToMaster);
   
  strip.begin(); // 네오픽셀 제어 시작
  strip.show(); // 네오픽셀 점등
  
  Serial.begin(9600); 
}

void loop () {
  cds = analogRead(A3);
  delay(1000);
  Serial.println(cds);
  if(cds<220){
  strip.begin();
  strip.setBrightness(100);
  strip.setPixelColor(0,255,200,200);// 첫째, 둘째, 셋제,  G(GREEN),R(RED), B(BLUE),0~255사이 표현 
  strip.setPixelColor(1,255,200,200); 
  strip.show();
  }else{
  strip.begin();
  strip.setBrightness(255);
  strip.setPixelColor(0,150,255,0);// 첫째, 둘째, 셋제,  G(GREEN),R(RED), B(BLUE),0~255사이 표현 
  strip.setPixelColor(1,150,255,0); 
  strip.show();
  }
 
}

void sendToMaster() {
 if(cds>220){
     Wire.write(mes); // 카운터 값을 증가시키고 마스터로 전송
  }else
     Wire.write(mes2);
}
