#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "../utils/queue.h"

int yylex();
int yyerror(char *message);

typedef struct HttpRequest_ {
	char* type;
	char* resource;
	char* version;
    Queue* attr_list;
} HttpRequest;

void create_request();

HttpRequest parse_request();

Queue* get_field(HttpRequest* request, char* field);

int get_status();

#endif
