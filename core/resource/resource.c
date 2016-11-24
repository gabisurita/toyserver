
#include "resource.h"
#include "../server.h"

int test_resource(char *__server_root, char *resource)
{
    char* full_path = (char*)malloc((strlen(__server_root)
                                     +strlen(resource)+1)*sizeof(char));

    // 1. Concatena caminho
    sprintf(full_path, "%s%s", __server_root, resource);

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
