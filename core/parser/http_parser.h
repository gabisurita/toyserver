#ifndef PARSER_H
#define PARSER_H

#include "../utils/queue.h"

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
