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