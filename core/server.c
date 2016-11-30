#include "server.h"

char* __server_log;

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

void __log(FILE* request, FILE* response, int verbose)
{

    FILE* log = fopen(__server_log, "a");

    fseek(log, 0L, SEEK_END);

    char buf[PACKET_SIZE];

    fprintf(log, "--- REQUEST ---\r\n\r\n");
    buf[0] = '\0';
    rewind(request);

    while((buf[0] != '\r') && !feof(request)){
        fgets(buf, sizeof(buf), request);
	    fprintf(log, buf);
    }

    if(verbose)
        fprintf(stdout, "\r\n\r\n");

	fprintf(log, "\r\n--- RESPONSE ---\r\n\r\n");
    buf[0] = '\0';
    rewind(response);

    while((buf[0] != '\r') && !feof(response)){
        fgets(buf, sizeof(buf), response);
	    fprintf(log, buf);

        if(verbose)
            fprintf(stdout, buf);

    }

	fprintf(log,"\n********************************************************************\r\n");

    fclose(log);
}


void* client_handler(void* args){

    int clientfd = *((int*)args);

    char request_buffer[PACKET_SIZE];
    char response_buffer[PACKET_SIZE];

    int n = read(clientfd, request_buffer, sizeof(request_buffer));

    // Read socket to mock file in memory
    FILE* request_file = fmemopen(request_buffer,
                              strlen(request_buffer), "r");

    // Set request as parser input
    yyin = request_file;

    // Create mock response file
    FILE* response_file = fmemopen(response_buffer,
                                   sizeof(response_buffer), "w");

    // Alocate request
    create_request();

    // call parser
    if(!yyparse())
    {
        // Parse request
        HttpRequest http_request = parse_request();
	    HttpRequest* requestList = &http_request;

        // Build response
        callback(requestList, request_file, response_file);

        // Send response
        fclose(response_file);
        puts(response_buffer);
        send(clientfd, response_buffer,
             strlen(response_buffer), 0);
    }

    response_file = fmemopen(response_buffer,
                             strlen(response_buffer), "r");

    __log(request_file, response_file, 1);

    fclose(request_file);
    fclose(response_file);

    close(clientfd);
}


int main(int argc, char **argv)
{
    // read port
    int port = atoi(argv[1]);

	// prepare webspace
    __server_root = argv[2];

    // log file
    __server_log = argv[3];

    // thread number
    int n_threads = atoi(argv[4]);


    // open socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // load server info
    struct sockaddr_in server_addr;
    bzero((char*) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind socket to server (assign port)
    bind(sockfd, (struct sockaddr*)&server_addr,
                 sizeof(server_addr));

    // make it a listening socket
    listen(sockfd, 20);

    pthread_t thread_ids[n_threads];
    int busy_threads[n_threads];

    // Busy thread handler
    for(int i=0; i<n_threads; i++)
        busy_threads[i] = 0;

    // Server loop
    while(1){

        int tno;

        // Get first free thread
        for(int tno=0; tno<=n_threads; tno++){
            if(busy_threads[tno] == 0)
                break;
        }

        // Server too busy, backoff
        if(tno == n_threads)
            continue;
        // Mark thread as busy
        else
            busy_threads[tno] = 1;

        // Create client connection
        struct sockaddr_in client_addr;
        int addr_len = sizeof(client_addr);
        int clientfd = accept(sockfd,
                              (struct sockaddr*)&client_addr,
                              &addr_len);

        // Call client handler
        void* args = (void*)&clientfd;
        pthread_create(&thread_ids[tno], NULL, client_handler, (void*)args);

        // Mark thread as free
        busy_threads[tno] = 0;
    }
}
