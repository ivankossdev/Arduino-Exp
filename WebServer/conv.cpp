#include "conv.h"

void DataConvertor::stringToIPaddress(String ipAddr){

  unsigned int len = ipAddr.length(); 
  unsigned int i = 0;  

  int oldRes = 0; 
  int res = 0; 
  int arrayItem = 0; 
  int hop = 0;  


  while(i < len){

    do{
      if(ipAddr.c_str()[i] != '.'){
        res = ipAddr.c_str()[i] - 0x30;
        
        if(hop == 0){
          oldRes = res; 
        } else if (hop > 0){
          res = oldRes * 10 + res;
          oldRes = res;
        }
        hop++;
      }
      i++;
    } while ( ipAddr.c_str()[i] != '.' && i < len); 

    hop = 0; 
    // Serial.printf("[%d] %d ", arrayItem, res);
    ip[arrayItem] = res;
    arrayItem++;
  }

  Serial.printf("\n");
  
}