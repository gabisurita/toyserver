%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

/* Tipo comando */
typedef struct comando{
  char* texto;
  Queue* filaParam;
} comando;

/* Fila de comandos */
Queue* filaComandos;

/* Comando atual para insercao de parametros */
comando* comandoAtual;

/* Contador de Linha */
int nlinhas = 1;

/* Remove ocorrencias de caracter em string */
void clearString(char* input, char rem);



%}

%union {
  char text[64];
}

%token 
COMANDO
PARAMETRO
VIRGULA
INVALIDO
NOVALINHA


%%
prog:
linhas | linha;

linhas:
linha linhas | linha;

linha:
comando | parametros | fimLinha;

parametros:
parametro VIRGULA parametros | parametro NOVALINHA;

comando:
COMANDO {
  
  /* Limpa a string de ":" e " " */
  clearString(yylval.text, ' ');
  clearString(yylval.text, ':');
  
  
  /* Cria novo comando */
  comandoAtual = (comando*)malloc(sizeof(comando));
  char* texto = (char*)malloc(sizeof(char)*strlen(yylval.text));
  strcpy(texto, yylval.text);
  comandoAtual->texto = texto;
  comandoAtual->filaParam = newQueue();
  
  /* Insere comando na fila */
  inQueue(&filaComandos, (char*)comandoAtual);

};

parametro:
PARAMETRO {
  
  /* Verifica se o comando atual e valido */
  if(comandoAtual){
    /* Cria novo parametro */
    char* texto = (char*)malloc(sizeof(char)*strlen(yylval.text));
    strcpy(texto, yylval.text);
    
    /* Insere parametro na fila */
    Queue* parametros = comandoAtual->filaParam; 
    inQueue(&parametros, texto);
  }
  /* Caso: parametro em comando invalido */
  else{
    printf("Comando inválido na linha %d\n", nlinhas);
  }
};

fimLinha:
NOVALINHA{
  nlinhas++;
  comandoAtual = NULL;
};

%%


int main(){

  filaComandos = newQueue();
  yyparse();
  
  /* Imprime conteudo da lista */
  while(notNullQueue(&filaComandos)){
    
    comando* iter = (comando*)outQueue(&filaComandos);
    
    char* texto = iter->texto;
    printf("%s : ", texto);
    free(texto);
    
    Queue* params = iter->filaParam;
    
    while(notNullQueue(&params)){
      char* texto = outQueue(&params);
      printf("%s, ", texto);
      free(texto);
    }
    
    destroyQueue(&params);
    free(iter);
    printf("\n");
  }
  
  destroyQueue(&filaComandos);
}


/* Remove ocorrencias de caracter em string
    Adaptado de http://stackoverflow.com/questions/4161822/ */
void clearString(char* input, char rem){

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

