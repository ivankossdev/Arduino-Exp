void setup() {
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
}

void port(int data){
  int temp = 0;
  
  for(int i = 0; i < 4; i++){
    temp = ~(data >> i) & 0x01;
    switch(i){
      case 0: digitalWrite(10, temp); break;
      case 1: digitalWrite(11, temp); break;
      case 2: digitalWrite(12, temp); break;
      case 3: digitalWrite(13, temp); break;
    }
  }
}

void loop() {
  port(0b000000011);
  delay(500);
  port(0x00);
  delay(500);
  port(0b00001001);
  delay(500);
  port(0x00);
  delay(500);
}
