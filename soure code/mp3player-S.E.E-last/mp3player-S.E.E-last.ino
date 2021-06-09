#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; //  SD카드를 사용하기 위한 오브젝트를 생성합니다.
SFEMP3Shield MP3player; //  Mp3 라이브러리를 사용하기 위한 오브젝트를 생성합니다.

const uint8_t volume = 100; // MP3 Player volume 0=max, 255=lowest (off)
const uint16_t monoMode = 1;  // Mono setting 0=off, 3=max
const int prePin = 3;   //이전 곡 재생 스위치
const int NextPin = 4; // 다음 곡 재생 스위치
const int stopPin = 5; //곡 재생 멈춤 스위치
int pre_buttonState = 0;
int Next_buttonState = 0;
int stop_buttonState = 0;

char tag1[13] = "82003B7C3DF8";   // RFID 태그의 ID입력
void setup()
{
  Serial.begin(9600);
  pinMode(prePin, INPUT); //di 3번
  pinMode(NextPin, INPUT); //di 4번
  pinMode(stopPin, INPUT); //di 5번
  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}
void loop()
{
  char tagString[13]; //  tagString를 선언합니다
  int index = 0 ;
  int sametag ;       //1이면 노래 재생 권한 부여

  int musicNOW = 2; // 처음 재생 곡 track 번호
  boolean reading = false;

  while (Serial.available()) {      // 시리얼 포트에 있는 태그 번호 (데이터)를 저장합니다.
    int readByte = Serial.read();  // RFID 태그의 ID를 저장합니다.
    tagString[index] = readByte;
    index++;
  }


  sametag = checkTag(tagString, musicNOW); // 함수 - 저장된 태그 ID와 RFID 리더에서 읽은 태그 ID와 일치하는지 확인 & 처음 트랙 재생

  if (sametag == 1) // 처음 곡이 재생되면
  {
    while (1) // 스위치로 곡 재생 변환 권한 부여 loop
    {
      Serial.print("와일문\n");
      pre_buttonState = digitalRead(prePin);
      Next_buttonState = digitalRead(NextPin);
      stop_buttonState = digitalRead(NextPin);
      if (Next_buttonState == HIGH) //다음 곡 재생
      {
        musicNOW++;
        MP3player.playTrack(musicNOW);
        Serial.print(musicNOW);
        Serial.print("다음 음악재생 성공\n");
        //delay(100000);
      }
      else if (pre_buttonState == HIGH)
      { musicNOW--;
        MP3player.playTrack(musicNOW);
        Serial.print(musicNOW);
        Serial.print("이전 음악재생 성공\n");
        //delay(15000);
      }
      else if (stop_buttonState == HIGH)
      {
        MP3player.stopTrack();
        Serial.print(musicNOW);
        Serial.print("음악 멈추기 성공\n");
        break; // stop스위치 누르면 와일문 탈출= 다시 태그 기다리는 loop문으로
      }
      Serial.print("아무 버튼 안 누름\n");
      Serial.print(musicNOW);
    }

    Serial.print("와일문 탈출 \n");
    Serial.print(musicNOW);
  }

  if (sametag == -1)
    Serial.print("태그 저장 실패\n");
  
  clearTag(tagString);
}

void initSD() {
  if (!sd.begin(SD_SEL, SPI_HALF_SPEED))
    sd.initErrorHalt();
  if (!sd.chdir("/"))
    sd.errorHalt("sd.chdir");
}

void initMP3Player() {
  uint8_t result = MP3player.begin();
  if (result != 0)
  {
  }
  MP3player.setVolume(volume, volume);
  MP3player.setMonoMode(monoMode);
}


int checkTag(char tag[], int musicNOW) {
  // 저장된 태그 ID 와 RFID 리더에서 읽은 태그 ID가 일치하는지 확인 하는 함수
  if (strlen(tag) == 0 ) { // 아무것도 읽지 않았을 경우
    return -1;
  }

  if (compareTag(tag, tag1)) //태그가 일치하면
  {
    Serial.print("태그 일치 \n");
    MP3player.playTrack(musicNOW);
    Serial.print("1 트랙 재생 성공\n");
    return 1;
  }
}

void clearTag(char one[]) {  // 읽은 태그 ID를 초기화
  for (int i = 0; i < strlen(one); i++) {
    one[i] = 0;
  }
}

boolean compareTag(char one[], char two[]) { // 읽은 태그 ID와 저장된 태그 ID를 비교
  if (strlen(one) == 0 ) { // 읽은 태그가 비어 있을 경우
    return false;
  }
  for (int i = 0; i < 11; i++) { // 읽은 태그와 저장된 태그 ID를 한자리 씩 비교

    if (one[i] != two[i]) {
      return false; // 하나라도 다르면 다른 태그 ID로 정함
    }
  }
  return true; // 전부 일치할 경우 일치하는 상태 값을 보냄
}

