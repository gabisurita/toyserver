%{
#include "../server.h"
#include "http_parser.h"

/* Tipo comando */
typedef struct comando{
  char* texto;
  Queue* filaParam;
} comando;

char* req  = NULL;
char* path = NULL;
char* protocolo = NULL;

/* Fila de comandos */
Queue* filaComandos;

/* Comando atual para insercao de parametros */
comando* comandoAtual;

/* Contador de Linha */
int __nlinhas = 1;


void __save_value();
char __buffer[PACKET_SIZE];
int __status = 0;

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
        clear_string(texto, ':');
        comandoAtual->texto = texto;
        comandoAtual->filaParam = new_queue();

        /* Insere comando na fila */
        in_queue(&filaComandos, (char*)comandoAtual);
    }
    ESPACO
    valores
;

valores:
    | valores valor;


valor:
    FIM
    | PARAMETRO{
        strcat(__buffer, yylval.text);
    }

    | ESPACO{
        strcat(__buffer, yylval.text);
    }

    | LITERAL{
        strcat(__buffer, yylval.text);
    }
    | VIRGULA{
        __save_value();
        __buffer[0] = '\0';
    }
    | NOVALINHA{
        __save_value();
        __buffer[0] = '\0';
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

void create_request(){
    filaComandos = new_queue();
}

HttpRequest parse_request(){
    HttpRequest request;
    request.type = req;
    request.version = protocolo;
    request.resource = path;
    request.attr_list = filaComandos;

    return request;
}

int get_status(){
    return __status;
}

Queue* get_field(HttpRequest* request, char* field){

    Node* head = request->attr_list->start;
    Node* iter = NULL;

    for(iter = head; iter->next != head; iter = iter->next){
        comando* cmd = (comando*)iter->val;

        if(!strcmp(cmd->texto, field))
            return cmd->filaParam;
    }

    // Check last node outside loop
    comando* cmd = (comando*)iter->val;

    if(!strcmp(cmd->texto, field))
        return cmd->filaParam;
    else
        return NULL;
}

void __save_value(){
    /* Verifica se o comando atual e valido */
    if(comandoAtual){
        /* Cria novo parametro */
        char* texto = (char*)malloc(sizeof(char)*strlen(__buffer));
        strcpy(texto, __buffer);

        /* Insere parametro na fila */
        Queue* parametros = comandoAtual->filaParam;
        in_queue(&parametros, texto);
    }
    /* Caso: parametro em comando invalido */
    else{
        __status = 400;
    }
}
