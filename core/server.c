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
*	response_code: permission code to resource
* 	requestFile: original request file
* 	responseFile: build response into it
*/
void _build_response(HttpRequest *requestList, char *request,
                   int response_code, FILE* requestFile, FILE* responseFile)
{
    // print Protocol version and response code
    fprintf(responseFile, "HTTP/1.1 %d ", response_code);

    // Print response label
    switch (response_code)
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

	if (!strcmp(request, "GET"))
	{
        //XXX: We print only HTML
		fprintf(responseFile, "Content-Type: text/html\r\n");

	    if(response_code == OK){

		    struct stat metadata = get_resource_status();
            fprintf(responseFile, "Content-Length: %d\r\n", (int)metadata.st_size);

            // print last modified date from "__resource_path"
            struct tm tm = *gmtime(&metadata.st_ctime);
            strftime(time_dump, sizeof(time_dump), "%a, %d %b %Y %H:%M:%S %Z", &tm);
			fprintf(responseFile, "Last-Modified: %s\r\n", time_dump);

            // Finish header
            fprintf(responseFile, "\r\n");

            // Output content
		    int fid;
		    char* path = get_resource_path();
            fid = open(path, O_RDONLY);
		    read(fid, path, metadata.st_size);
		    fwrite(path, sizeof(char), metadata.st_size, responseFile);
		    close(fid);
		}

	    if(response_code == NOT_FOUND || response_code == FORBIDDEN
                                      || response_code == BAD_REQUEST
                                      || response_code == METHOD_NOT_IMPLEMENTED){

		    struct stat metadata = get_resource_status();
            fprintf(responseFile, "Content-Length: 0\r\n");

            // Finish header
            fprintf(responseFile, "\r\n");
		}
	    //case NOT_FOUND,FORBIDDEN, etc
	}

	if (!strcmp(request, "HEAD"))
	{
        //XXX: We print only HTML
		fprintf(responseFile, "Content-Type: text/html\r\n");

	    if(response_code == OK){
		    struct stat metadata = get_resource_status();
            fprintf(responseFile, "Content-Length: %d\r\n", (int)metadata.st_size);

			// print last modified date from "__resource_path"
            struct tm tm = *gmtime(&metadata.st_ctime);
            strftime(time_dump, sizeof(time_dump), "%a, %d %b %Y %H:%M:%S %Z", &tm);
			fprintf(responseFile, "Last-Modified: %s\r\n", time_dump);

            // Finish header
            fprintf(responseFile, "\r\n");
		}

	    if(response_code == NOT_FOUND || response_code == FORBIDDEN
                                      || response_code == BAD_REQUEST
                                      || response_code == METHOD_NOT_IMPLEMENTED){

            struct stat metadata = get_resource_status();
            fprintf(responseFile, "Content-Length: 0\r\n");

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

void __log(FILE* log, FILE* request, FILE* response, int verbose)
{
	fseek(log, 0L, SEEK_END);

    char buf[1024];

    fprintf(log, "--- REQUEST ---\r\n\r\n");
    buf[0] = '\0';
    rewind(request);

    while((buf[0] != '\r') && !feof(request)){
        fgets(buf, 1024, request);
	    fprintf(log, buf);
    }

    if(verbose)
        fprintf(stdout, "\r\n\r\n");

	fprintf(log, "\r\n--- RESPONSE ---\r\n\r\n");
    buf[0] = '\0';
    rewind(response);

    while((buf[0] != '\r') && !feof(response)){
        fgets(buf, 1024, response);
	    fprintf(log, buf);

        if(verbose)
            fprintf(stdout, buf);

    }

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

	__log(log, request, response, 1);
}
