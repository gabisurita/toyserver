%{
#include <stdio.h>
#include <string.h>

char variaveis[64][64];
int nVar = 0;
%}

%union {
  char text[64];
}

%token 
INICIO_MAIN 
FIM_MAIN 
PONTO_VIRGULA 
TIPO
VIRGULA 
NOMEVAR 
IGUAL 
VALOR 
OPERA 
PARENTESES_ESQ 
PARENTESES_DIR

%%
prog_fonte:
INICIO_MAIN conteudo_prog FIM_MAIN;

conteudo_prog:
declaracoes expressoes;

declaracoes:
linha_declara declaracoes | linha_declara;

linha_declara: 
TIPO variaveis PONTO_VIRGULA;

variaveis:
identificador VIRGULA variaveis | identificador;

identificador:
NOMEVAR | atribuicao IGUAL VALOR ;

expressoes:
linha_executavel expressoes | linha_executavel;

linha_executavel:
atribuicao IGUAL operacoes PONTO_VIRGULA /* idem */;

operacoes:
operacoes OPERA operacoes 
| PARENTESES_ESQ operacoes PARENTESES_DIR 
| VALOR
| acesso;


atribuicao:
NOMEVAR {
  strcpy(variaveis[nVar], yylval.text);
  nVar++;
};


acesso:
NOMEVAR {
  int ui = 1;
  for(int i=0; i<nVar; i++){
    if(strcmp(yylval.text, variaveis[i]) == 0){
      ui = 0;
      break;
    }
  }
  if(ui)
    printf("Aviso: %s nao inicializada\n", yylval.text); 
};

%%

int main(){
  yyparse();
}
