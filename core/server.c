#include "server.h"

/* Function server_callback()
* Answera a given a request
*
* Parameters:
*	requestList: request to be answered
* 	request: original request file
* 	response: file to save response
*/
void callback(HttpRequest *requestList, FILE *request, FILE* response)
{
    int code = get_status();
    if (code){
        _build_response(requestList, "GET", code, request, response);
    }
	if (!strcmp(requestList->type, "GET"))
	{
		code = test_resource(__server_root, requestList->resource);
        _build_response(requestList, "GET", code, request, response);
	}
	if (!strcmp(requestList->type, "HEAD"))
	{
		code = test_resource(__server_root, requestList->resource);
        _build_response(requestList, "HEAD", code, request, response);
    }
}
/* Function setServerRoot()
* Sets the server's root directory
*
* Parameters:
*	char *root: path to root directory
*/

void _set_server_root(char *root)
{
	sprintf(__server_root, "%s", root);
}


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

/* Function __log()
* Adds request/response to file log
*
* Parameters:
*	log : log file
*	request: request file
*	response: response file
*/

void __log(FILE* log, FILE* request, FILE* response)
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
	_set_server_root(argv[1]);

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
        callback(requestList, request, response);
	}
	//else ERROR

	__log(log, request, response);
}
