#include "resource.h"
#include "../server.h"

struct stat __resource_status;
char __resource_path[256];

int test_resource(char *__server_root, char *resource)
{
    char* full_path = (char*)malloc((strlen(__server_root)
                                     +strlen(resource)+1)*sizeof(char));

    // 1. Concatena caminho
    sprintf(full_path, "%s%s", __server_root, resource);

    // 2. Busca estado do caminho
    if(stat(full_path, &__resource_status) < 0)
        return NOT_FOUND;

    // 3. Checa permissao de leitura
    if(!(__resource_status.st_mode & S_IRUSR)){
        return FORBIDDEN;
    }

    // 4.1 Caminho é um arquivo
    if(S_ISREG(__resource_status.st_mode)){
        int file_desc;

        // 4.1.1 Abre arquivo com open()
        if((file_desc = open(full_path, O_RDONLY)) == -1){
            return SERVER_ERROR;
        }

        strcpy(__resource_path, full_path);
    }
    // 4.2 Caminho é um diretório
    else{

        // 4.2.1 Verifica se diretorio permite varredura
        if(!(__resource_status.st_mode & S_IXUSR)){
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

        // 4.2.2 Verifica se os arquivos existem
        if(stat(index_path, &__resource_status) < 0){
            if(stat(welcome_path, &__resource_status) < 0)
                return NOT_FOUND;
        }

        int file_desc;

        // 4.2.3 Tenta ler index.html
        if((file_desc = open(index_path, O_RDONLY)) == -1){
            if((file_desc = open(welcome_path, O_RDONLY)) == -1)
                return FORBIDDEN;
            else
                strcpy(__resource_path, welcome_path);
        }
        else{
            strcpy(__resource_path, index_path);
        }

        free(index_path);
        free(welcome_path);
    }

    free(full_path);
    return OK;
}

struct stat get_resource_status(){
    return __resource_status;
}

char* get_resource_path(){
    return __resource_path;
}
