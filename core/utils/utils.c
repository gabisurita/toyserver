#include "utils.h"

/* Remove ocorrencias de caracter em string
    Adaptado de http://stackoverflow.com/questions/4161822/ */
void clear_string(char* input, char rem){

  char *src, *dest;
  src = dest = input;

  while(*src != '\0'){
      if(*src != rem){
          *dest = *src;
          dest++;
      }
      src++;
  }
  *dest = '\0';
}

