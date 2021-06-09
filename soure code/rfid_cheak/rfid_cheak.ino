void setup() {
  Serial.begin(9600);
  Serial.println("RFID example start!");
  Serial.println('\n');
}

void loop() {
  // 태그의 ID를 저장할 곳
  char tagString[15];  
  int index = 0;
  // 시리얼 포트에 읽을 데이터가 있으면
  while(Serial.available()) {  
    // 시리얼 포트에 있는 데이터를 저장합니다.
    int readByte = Serial.read(); 
    Serial.println("Serial.read() is ");
    Serial.print("DEC : ");
    Serial.println(readByte, DEC);
    Serial.print("HEX : ");
    Serial.println(readByte, HEX);
    Serial.println("RFID reading.... store string");
    
    // RFID 태그의 ID를 저장합니다.
    tagString[index] = readByte;  
    index++;
    Serial.println("tagString is : ");
    // RFID 태그의 ID를 출력합니다.
    Serial.println(tagString); 
  }
}
