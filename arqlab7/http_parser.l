%{
  #include "http_parser.tab.h"
%}

%%

\r\n\r                  {return FIM;}
\n\n                    {return FIM;}
\r\r                    {return FIM;}
\r\n                    {return NOVALINHA;}
\n                      {return NOVALINHA;}
\r                      {return NOVALINHA;}
,                       {return VIRGULA;}
[ \t]*                  {strcpy(yylval.text, yytext); return ESPACO;}
\"(\\.|[^"])*\"         {strcpy(yylval.text, yytext); return LITERAL;}
\((\\.|[^)])*\)         {strcpy(yylval.text, yytext); return LITERAL;}
[^,\n\t\r ]+:           {strcpy(yylval.text, yytext); return CHAVE;}
[^,\n\t\r ]+            {strcpy(yylval.text, yytext); return PARAMETRO;}
%%

