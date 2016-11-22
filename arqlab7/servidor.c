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

#include "http_parser.h"
#include "http.tab.h"

extern FILE* yyin;

static char serverRoot[256];

#define PATH_OFFSET 20

#define OK	200
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define SERVER_ERROR 500
#define METHOD_NOT_IMPLEMENTED 501

static char __resource_path[256];

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
void buildResponse(HttpRequest *requestList, char *request,
                   int responseCode, FILE* requestFile, FILE* responseFile)
{
    // print Protocol version and response code
    fprintf(responseFile, "HTTP/1.1 %d ", responseCode);

    // Print response label
    switch (responseCode)
	{
		case OK:
			fprintf(responseFile, "OK\r\n");
			break;
		case BAD_REQUEST:
			fprintf(responseFile, "Bad Request\r\n");
			break;
		case FORBIDDEN:
			fprintf(responseFile, "Forbidden\r\n");
			break;
		case NOT_FOUND:
			fprintf(responseFile, "Not Found\r\n");
			break;
		case METHOD_NOT_IMPLEMENTED:
			fprintf(responseFile, "Method Not Implemented\r\n");
			break;
	}

	// Get connection fiels
    Queue* values = get_field(requestList, "Connection");
    // If "Connection" is not available, close is used by default
    char* conn = "close";
    // Last defined value is used by default
    if(values){
        conn = out_queue(&values);
    }
    fprintf(responseFile, "Connection: %s\r\n", conn);

    // Print local time (in GMT)
    char time_dump[64];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(time_dump, sizeof(time_dump), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    fprintf(responseFile, "Date: %s\r\n", time_dump);
	fprintf(responseFile, "Server: GabiToyServer/0.0.1\r\n");

	if (request=="GET")
	{
        //XXX: We print only HTML
		fprintf(responseFile, "Content-Type: text/html\r\n");

		struct stat statbuff;
		stat(__resource_path, &statbuff);
		fprintf(responseFile, "Content-Length: %d\r\n", (int)statbuff.st_size);

	    if(responseCode == OK){
			// print last modified date from "__resource_path"
            struct tm tm = *gmtime(&statbuff.st_ctime);
            strftime(time_dump, sizeof(time_dump), "%a, %d %b %Y %H:%M:%S %Z", &tm);
			fprintf(responseFile, "Last-Modified: %s\r\n", time_dump);

            // Finish header
            fprintf(responseFile, "\r\n");

            // Output content
		    int fid;
		    fid = open(__resource_path, O_RDONLY);
		    read(fid, __resource_path, statbuff.st_size);
		    fwrite(__resource_path, sizeof(char), statbuff.st_size, responseFile);
		    close(fid);
		}

	    if(responseCode == NOT_FOUND || responseCode == FORBIDDEN
                                     || responseCode == BAD_REQUEST
                                     || responseCode == METHOD_NOT_IMPLEMENTED){
            // Finish header
            fprintf(responseFile, "\r\n");
		}
	    //case NOT_FOUND,FORBIDDEN, etc
	}

	if (request=="HEAD")
	{
        //XXX: We print only HTML
		fprintf(responseFile, "Content-Type: text/html\r\n");

		struct stat statbuff;
		stat(__resource_path, &statbuff);
		fprintf(responseFile, "Content-Length: %d\r\n", (int)statbuff.st_size);

	    if(responseCode == OK){
			// this is only an example, date from "__resource_path"
			//fprintf(responseFile, "Last-Modified: Sat, 18 Oct 2014 18:40:44 BRT\r\n");
			// print last modified date from "__resource_path"
            struct tm tm = *gmtime(&statbuff.st_ctime);
            strftime(time_dump, sizeof(time_dump), "%a, %d %b %Y %H:%M:%S %Z", &tm);
			fprintf(responseFile, "Last-Modified: %s\r\n", time_dump);
		}

	    if(responseCode == NOT_FOUND || responseCode == FORBIDDEN
                                     || responseCode == BAD_REQUEST
                                     || responseCode == METHOD_NOT_IMPLEMENTED){
            // Finish header
            fprintf(responseFile, "\r\n");
		}
	}

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
    int code = get_status();
    if (code){
        buildResponse(requestList, "GET", code, request, response);
    }
	if (!strcmp(requestList->type, "GET"))
	{
		code = testResource(serverRoot, requestList->resource);
        buildResponse(requestList, "GET", code, request, response);
	}
	if (!strcmp(requestList->type, "HEAD"))
	{
		code = testResource(serverRoot, requestList->resource);
        buildResponse(requestList, "HEAD", code, request, response);
    }
}

/* Function testResource()
* Verify if a resource on a given path exists and if it is accessable
*
* Parameters:
*   ServerRoot: webspace root address
*
*/
int testResource(char *serverRoot, char *resource)
{

    char* full_path = (char*)malloc((strlen(serverRoot)
                                     +strlen(resource)+1)*sizeof(char));

    // 1. Concatena caminho
    sprintf(full_path, "%s%s", serverRoot, resource);

    struct stat path_status;

    // 2. Busca estado do caminho
    if(stat(full_path, &path_status) < 0)
        return NOT_FOUND;

    // 3. Checa permissao de leitura
    if(!(path_status.st_mode & S_IRUSR)){
        return FORBIDDEN;
    }

    // 4.1 Caminho é um arquivo
    if(S_ISREG(path_status.st_mode)){
        int file_desc;

        // 4.1.1 Abre arquivo com open()
        if((file_desc = open(full_path, O_RDONLY)) == -1){
            return SERVER_ERROR;
        }

        char buf[2048];
        int n;

        // 4.1.2 Escreve na stdout
        while((n = read(file_desc,buf,sizeof(buf))) != 0){
            fflush(stdout);
            write(1,buf,n);
        }

    }
    // 4.2 Caminho é um diretório
    else{

        // 4.2.1 Verifica se diretorio permite varredura
        if(!(path_status.st_mode & S_IXUSR)){
            return FORBIDDEN;
        }

        // Monta caminho do index.html
        char* index_path = (char*)malloc((strlen(full_path)
                                          +strlen("index.html")+1)*sizeof(char));

        // Monta caminho do welcome.html
        char* welcome_path = (char*)malloc((strlen(full_path)
                                            +strlen("welcome.html")+1)*sizeof(char));

        sprintf(index_path, "%sindex.html", full_path);
        sprintf(welcome_path, "%swelcome.html", full_path);

        struct stat path_status;

        // 4.2.2 Verifica se os arquivos existem
        if(stat(index_path, &path_status) < 0){
            if(stat(welcome_path, &path_status) < 0)
                return NOT_FOUND;
        }

        int file_desc;

        // 4.2.3 Tenta ler index.html
        if((file_desc = open(index_path, O_RDONLY)) == -1){

            // Se nao existe index.html, verifica welcome.html
            if((file_desc = open(welcome_path, O_RDONLY)) == -1)
                return FORBIDDEN;
        }

        char buf[2048];
        int n;

        // 4.2.4 Imprime o conteudo
        while((n = read(file_desc,buf,sizeof(buf))) != 0){
            fflush(stdout);
            write(1,buf,n);
        }

        free(index_path);
        free(welcome_path);
    }

    strcpy(__resource_path, full_path);
    free(full_path);
    return OK;
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

int main(int argc, char **argv)
{
	HttpRequest *requestList;
	FILE *request, *response, *log;

	// prepare webspace
	httpServer_setServerRoot(argv[1]);

	yyin = fopen(argv[2], "r");
	request = yyin;

	response = fopen(argv[3], "w+");

	log = fopen(argv[4], "a");

	create_request();

	// call parser
	if(!yyparse())
	{
        HttpRequest http_request = parse_request();
		requestList = &http_request;
        httpServer_answerRequest(requestList, request, response);
	}
	//else ERROR

	httpServer_addToLog(log, request, response);
}
