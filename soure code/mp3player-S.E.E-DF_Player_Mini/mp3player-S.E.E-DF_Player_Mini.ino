#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; //  SD카드를 사용하기 위한 오브젝트를 생성합니다.
SFEMP3Shield MP3player; //  Mp3 라이브러리를 사용하기 위한 오브젝트를 생성합니다.

const uint8_t volume = 0; // MP3 Player volume 0=max, 255=lowest (off)
const uint16_t monoMode = 1;  // Mono setting 0=off, 3=max
const int prePin = 4;   //이전 곡 재생 스위치
const int NextPin = 3; // 다음 곡 재생 스위치
const int resetPin = 5;

int pre_buttonState = 0;
int Next_buttonState = 0;
int reset_buttonState = 0;

int musicNOW = 1;

char tag1[13] = "82003B7C3DF8";   // RFID 태그의 ID입력
void setup()
{
  Serial.begin(9600);
  pinMode(prePin, INPUT); //di 3번
  pinMode(NextPin, INPUT); //di 4번
  pinMode(resetPin, INPUT); //di 5번
 
  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}
void loop()
{
  char tagString[13]; //  tagString를 선언합니다
  int index = 0 ;
  int musicNOW1;
  boolean reading = false;



  while (Serial.available()) { //시리얼 포트에 읽을 데이터가 있으면
    // 태그 리더기에 태그를 가져다 대면, 읽기 시작합니다.

    int readByte = Serial.read(); // 시리얼 포트에 있는 데이터를 저장합니다.// 태그에서 읽은 정보를 출력
    Serial.println("Serial.read() is ");
    Serial.print("DEC : "); Serial.println(readByte, DEC  );
    Serial.print("HEX : "); Serial.println(readByte, HEX);
    Serial.println("RFID reading..... store string");
    if (readByte >= '0' && readByte <= 'F') {
      Serial.print("readByte input : ");
      Serial.println(readByte);
      tagString[index] = readByte;
      index++;
    }
  }


  musicNOW1 = checkTag(tagString, musicNOW); // 함수 - 저장된 태그 ID와 RFID 리더에서 읽은 태그 ID와 일치하는지 확인 & 처음 트랙 재생
  if (musicNOW1 > 0)
  {
    musicNOW = musicNOW1;
  }
  clearTag(tagString);b
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
    return -1 ;
  }

  if (compareTag(tag, tag1)) //태그가 일치하면
  {
    Serial.print("태그 일치 \n");
  
    MP3player.playTrack(musicNOW);
    Serial.print("1 트랙 재생 성공\n");
    delay(3000);

    pre_buttonState = digitalRead(prePin); //3번
    Next_buttonState = digitalRead(NextPin);//4번
    reset_buttonState = digitalRead(resetPin);//5번

    if (pre_buttonState == HIGH) //&& (Next_buttonState == HIGH)  && (reset_buttonState == HIGH))
    {
      
      MP3player.stopTrack();
      musicNOW--;
      if (musicNOW == 0)
      {
        musicNOW = 9;
      }
      MP3player.playTrack(musicNOW);
      
      Serial.print("현재 음악 번호:");
      Serial.print(musicNOW);
      Serial.print("이전 음악재생 성공\n");
      return musicNOW;
    }

    if (Next_buttonState == HIGH ) //&& (pre_buttonState == HIGH) && (reset_buttonState == HIGH))
    {
      
      MP3player.stopTrack();
      musicNOW++;
      if (musicNOW == 10)
      {
        musicNOW = 1;
      }
      MP3player.playTrack(musicNOW);
     
      Serial.print("현재 음악 번호:");
      Serial.print(musicNOW);
      Serial.print("다음 음악재생 성공\n");
      return musicNOW;
    }

    if (reset_buttonState == HIGH) //&& (Next_buttonState == HIGH) && (pre_buttonState == HIGH)) //&& (stop_buttonState == HIGH)))
    {
      
      MP3player.stopTrack();
      
      Serial.print("현재 음악 번호:");
      Serial.print(musicNOW);
      Serial.print("리셋\n");
      return musicNOW;
      // reset 버튼 누르면 탈출
    }
    else
    {
      Serial.print("버튼 누르지 않음\n");
      return musicNOW;
    }
  }
  else {
    
    return musicNOW;
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

