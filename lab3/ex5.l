%{
  #include "ex5.tab.h"
%}

%%
#.*                     {}
,                       {return VIRGULA;}
[^,: \n\t\r]+:          {strcpy(yylval.text, yytext); return COMANDO;}
[^,: \n\t\r]+           {strcpy(yylval.text, yytext); return PARAMETRO; }
[\n]                    {return NOVALINHA;}
.                       {}
%%

