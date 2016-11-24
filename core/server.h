#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include "parser/http_parser.h"
#include "parser/http_parser.tab.h"
#include "resource/resource.h"


#define PATH_OFFSET 20

#define OK	200
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define SERVER_ERROR 500
#define METHOD_NOT_IMPLEMENTED 501


extern FILE* yyin;

static char __server_root[256];

static char __resource_path[256];

/* Function server_callback()
* Answera a given a request
*
* Parameters:
*	requestList: request to be answered
* 	request: original request file
* 	response: file to save response
*/

void callback(HttpRequest *requestList, FILE *request, FILE* response);

/* Function set_server_root()
* Sets the server's root directory
*
* Parameters:
*	char *root: path to root directory
*/

void _set_server_root(char *root);

/* Function _build_response()
* Build response into a file
*
* Parameters:
*	request: request type code
*	responseCode: permission code to resource
* 	requestFile: original request file
* 	responseFile: build response into it
*/
void _build_response(HttpRequest *requestList, char *request,
                   int responseCode, FILE* requestFile, FILE* responseFile);
