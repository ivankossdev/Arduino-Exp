#include "conv.h"

bool DataConvertor::checkIpAddress(String ipAddr){
  int i = 0;
  int checkValue = 0; 
  bool status = false; 
  do{
    checkValue = ipAddr.c_str()[i] - 0x30; 
    if ((checkValue >= 0 && checkValue <= 9) || checkValue == -2) {
      status = true;
    } else {
      status = false;
      break; 
    }
    i++;
  }while(ipAddr.c_str()[i] != '\0');
  return status; 
}

void DataConvertor::stringToIPaddress(String ipAddr, int *ip){

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
    ip[arrayItem] = res;
    arrayItem++;
  }
}