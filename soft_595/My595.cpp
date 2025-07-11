#include "My595.h"

MY595::MY595(uint8_t clk, uint8_t st_cp, uint8_t data){
  this->clk = clk;
  this->st_cp = st_cp;
  this->data = data;

  pinMode(clk, OUTPUT);
  pinMode(st_cp, OUTPUT);
  pinMode(data, OUTPUT);

  digitalWrite(clk, LOW);
  digitalWrite(st_cp, LOW);
  digitalWrite(data, LOW);
}

void MY595::WriteBit(char _bit){
  digitalWrite(clk, LOW);
  digitalWrite(data, _bit);
  digitalWrite(clk, HIGH);
}

void MY595::WriteByte(char _byte){
  char temp = 0;
  digitalWrite(st_cp, HIGH);
  for(unsigned int i = 0; i < 8; i++){
    temp = (_byte >> i) & 0x01;
    WriteBit(temp);
  }
  digitalWrite(st_cp, LOW);
}