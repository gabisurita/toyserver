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

#include "parser.h"
#include "parser.tab.h"

extern FILE* yyin;

static char serverRoot[256];	

#define PATH_OFFSET 20

#define OK	200

static char *resourcePath;

/* Function setServerRoot()
* Sets the server's root directory
*
* Parameters:
*	char *root: path to root directory
*/

void httpServer_setServerRoot(char *root)
{
	sprintf(serverRoot, "%s", root);
}

/* Function buildResponse()
* Build response into a file
*
* Parameters:
*	request: request type code
*	responseCode: permission code to resource
* 	requestFile: original request file
* 	responseFile: build response into it
*/
void buildResponse(HttpRequest *requestList, char *request, int responseCode, FILE* requestFile, FILE* responseFile)
{
	fprintf(responseFile, "HTTP/1.1 %d ", responseCode);
	switch (responseCode)
	{
		case OK:
			fprintf(responseFile, "OK\r\n");
			break;
		//NOT_FOUND,FORBIDDEN,ETC
	}

	// current date, this is only an example
	fprintf(responseFile, "Date: Sun, 19 Oct 2014 08:55:36 BRST\r\n");

	fprintf(responseFile, "Server: Servidor HTTP ver 0.1 de XXX \r\n");

	//print connection type using requestList, this is only an example
	fprintf(responseFile, "Connection: keep-alive\r\n");	
	
	if (request=="GET")
	{
		fprintf(responseFile, "Content-Type: text/html\r\n");
		switch (responseCode)
		{				
			case OK:
				// this is only an example, date from "resourcePath"
				fprintf(responseFile, "Last-Modified: Sat, 18 Oct 2014 18:40:44 BRT\r\n");	
				break;
			//case NOT_FOUND,FORBIDDEN, etc
		}
			
		struct stat statbuff;
		stat(resourcePath, &statbuff);
		fprintf(responseFile, "Content-Length: %d\r\n\r\n", (int)statbuff.st_size);
		
		int fid;					
		fid = open(resourcePath, O_RDONLY);
		read(fid, resourcePath, statbuff.st_size);
		fwrite(resourcePath, sizeof(char), statbuff.st_size, responseFile);
		close(fid);
	}
	//else -> HEAD, ETC

}

/* Function server_answerRequest()
* Answera a given a request
*
* Parameters:
*	requestList: request to be answered
* 	request: original request file
* 	response: file to save response
*/
void httpServer_answerRequest(HttpRequest *requestList, FILE *request, FILE* response)
{	
	int code;

	if (!strcmp(requestList->type, "GET"))
	{
		code = testResource(serverRoot, requestList->resource);
		buildResponse(requestList, "GET", code, request, response);
	}
	//else if ("HEAD", "OPTIONS", etc)	
}

/* Function testResource()	
* Verify if a resource on a given path exists and if it is accessable 
* 
*/ 
int testResource(char *serverRoot, char *resource) 
{	
	int resourceSize = strlen(serverRoot) + strlen(resource) + PATH_OFFSET;
	resourcePath = (char *)malloc(resourceSize*sizeof(char));
	sprintf(resourcePath, "%s%s", serverRoot, resource);
	struct stat statbuf; 

	int returnValue; 

	if (stat(resourcePath, &statbuf) == -1) { 

	} 
	else { 
		switch (statbuf.st_mode & S_IFMT) { 			
			case S_IFREG: 
				returnValue = OK;
				break; 			
		}
	}

	return returnValue;
}

/* Function addToLog()
* Adds request/response to file log
*
* Parameters:
*	log : log file
*	request: request file
*	response: response file
*/

void httpServer_addToLog(FILE* log, FILE* request, FILE* response)
{
	fseek(log, 0L, SEEK_END);

	fprintf(log, "--- REQUEST ---\r\n\r\n");
	fprintf(log,"colocar aqui o conteúdo do arquivo 'request'");


	fprintf(log, "\r\n--- RESPONSE ---\r\n\r\n");
	fprintf(log,"colocar aqui o conteúdo do arquivo 'response' (mas só o cabeçalho da mesma, sem o conteúdo do recursos solicitado, no caso de GET)");
	
	fprintf(log,"\n********************************************************************\r\n");
}

main(int argc, char **argv) 
{ 
	HttpRequest *requestList; 
	FILE *request, *response, *log; 

	// prepare webspace 
	httpServer_setServerRoot(argv[1]); 

	yyin = fopen(argv[2], "r"); 
	request = yyin; 
	
	response = fopen(argv[3], "w+"); 

	log = fopen(argv[4], "a"); 

	// call parser 
	if(!yyparse()) 
	{ 
		requestList = httpParser_getRequestList(); 
		httpServer_answerRequest(requestList, request, response); 	
		httpParser_printRequestList(requestList); 
	} 
	//else ERROR
		
	httpServer_addToLog(log, request, response); 
}
