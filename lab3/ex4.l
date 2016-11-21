%{
  #include "ex4.tab.h"
%}

%%

"main() {"              {return INICIO_MAIN;}
"}"                     {return FIM_MAIN;}
int|char|float|double   {return TIPO;}
[a-zA-z][a-zA-Z0-9_]*   {strcpy(yylval.text, yytext);  return NOMEVAR;}
[0-9]*[.]*[0-9]*        {return VALOR;}
[=]                     {return IGUAL;}
[+"-""/""*"]            {return OPERA;}
;                       {return PONTO_VIRGULA;}
,                       {return VIRGULA;}
"("			                {return PARENTESES_ESQ;}
")"			                {return PARENTESES_DIR;}
\n                      {}
.                       {}

%%
