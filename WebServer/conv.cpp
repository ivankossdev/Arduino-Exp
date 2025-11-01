#include "conv.h"

void DataConvertor::stringToIPaddress(String ipAddr){
  unsigned int len = ipAddr.length(); unsigned int i = 0;
  while(i < len){
    do{
      if(ipAddr.c_str()[i] != '.')
        Serial.printf("[%d] %c\n", i, ipAddr.c_str()[i]);
      i++;
    } while ( ipAddr.c_str()[i] != '.' && i < len); 
    Serial.printf("\n");
  }
  
}