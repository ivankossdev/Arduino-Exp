
/*Arduino board >>    << 595 board*/
#define CLK 5  // --> 11 Clock 
#define STcp 6 // --> 12 Storage Registet input 
#define Data 9 // --> 14 Data pin



void setup() {
  pinMode(CLK, OUTPUT); 
  pinMode(STcp, OUTPUT);  
  pinMode(Data, OUTPUT); 
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  digitalWrite(CLK, LOW);
  digitalWrite(STcp, LOW);
  digitalWrite(Data, LOW);
}

void WriteBit(char _bit){
  digitalWrite(CLK, LOW);
  digitalWrite(Data, _bit);
  digitalWrite(CLK, HIGH);
}

void WriteByte(char _byte){
  char temp = 0;
  digitalWrite(STcp, HIGH);
  for(unsigned int i = 0; i < 8; i++){
    temp = (_byte >> i) & 0x01;
    WriteBit(temp);
  }
  digitalWrite(STcp, LOW);
}

void loop() {
  /*Write Bit*/
  for(int i = 0; i < 8; i++){
    
    WriteByte(1 << i);
    delay(500);
  }
}
