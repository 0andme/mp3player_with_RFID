#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

void setup () {
  Serial.begin (9600);
  mp3_set_serial (Serial);      // DFPlayer-mini mp3 module 시리얼 세팅
  delay(1);                     // 볼륨값 적용을 위한 delay
  mp3_set_volume (5);          // 볼륨조절 값 0~30
}

void loop () {
  mp3_play (1);    //0001 파일 플레이
  delay (6000);
  mp3_next ();    //0002 파일 플레이 , next
  delay (6000);
  mp3_prev ();    //0001 파일 플레이 , prev
  delay (6000);
  mp3_play (4);    //0004 파일 플레이
  delay (6000);
}
