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

char* req  = NULL;
char* path = NULL;
char* protocolo = NULL;
char* host = NULL;
int   porta = 80;

/* Fila de comandos */
Queue* filaComandos;

/* Comando atual para insercao de parametros */
comando* comandoAtual;

/* Contador de Linha */
int nlinhas = 1;

/* Remove ocorrencias de caracter em string */
void clearString(char* input, char rem);

void saveValue();

char buffer[1024];
char* buffer_entry = buffer;

%}

%union {
  char text[1024];
}

%token
COMANDO
PARAMETRO
VIRGULA
DOISPONTOS
NOVALINHA
HOST
ESPACO
FIM
CHAVE
LITERAL
%%
prog: requisicao atributos;

atributos:
    | atributos atributo
    ;

atributo:
    CHAVE{
        /* Cria novo comando */
        comandoAtual = (comando*)malloc(sizeof(comando));
        char* texto = (char*)malloc(sizeof(char)*strlen(yylval.text));
        strcpy(texto, yylval.text);
        clearString(texto, ':');
        comandoAtual->texto = texto;
        comandoAtual->filaParam = newQueue();

        /* Insere comando na fila */
        inQueue(&filaComandos, (char*)comandoAtual);
    }
    ESPACO
    valores
;

valores:
    | valores valor;


valor:
    FIM
    | PARAMETRO{
        strcat(buffer, yylval.text);
    }

    | ESPACO{
        strcat(buffer, yylval.text);
    }

    | LITERAL{
        strcat(buffer, yylval.text);
    }
    | VIRGULA{
        saveValue();
        buffer[0] = '\0';
    }
    | NOVALINHA{
        saveValue();
        buffer[0] = '\0';
    }


requisicao:
    PARAMETRO{
        char* texto = yylval.text;
        req =  (char*)malloc(sizeof(char)*strlen(texto));
        strcpy(req, texto);

    }
    ESPACO
    PARAMETRO{
        char* texto = yylval.text;
        path = (char*)malloc(sizeof(char)*strlen(texto));
        strcpy(path, texto);

    }
    ESPACO
    PARAMETRO{
        char* texto = yylval.text;
        protocolo = (char*)malloc(sizeof(char)*strlen(texto));
        strcpy(protocolo, texto);
    }
    NOVALINHA
    ;


%%


int main(){

  filaComandos = newQueue();
  yyparse();

  printf("Requisição: %s\n", req);
  printf("Local: %s\n", path);
  printf("Protocolo: %s\n", protocolo);
  printf("Porta: %d\n", porta);


  /* Imprime conteudo da lista */
  while(notNullQueue(&filaComandos)){

    comando* iter = (comando*)outQueue(&filaComandos);

    char* texto = iter->texto;
    printf("%s: ", texto);
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

  free(req);
  free(path);
  free(protocolo);
  free(host);
}

void saveValue(){
    /* Verifica se o comando atual e valido */
    if(comandoAtual){
        /* Cria novo parametro */
        char* texto = (char*)malloc(sizeof(char)*strlen(buffer));
        strcpy(texto, buffer);

        /* Insere parametro na fila */
        Queue* parametros = comandoAtual->filaParam;
        inQueue(&parametros, texto);
    }
    /* Caso: parametro em comando invalido */
    else{
        printf("Comando inválido na linha %d\n", nlinhas);
    }
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

