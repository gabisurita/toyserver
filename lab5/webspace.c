#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


#define NOT_FOUND 404
#define FORBIDDEN 403
#define SERVER_ERROR 500


int get_resource(char* webspace_path, char* resource_path){
  
  char* full_path = (char*)malloc((strlen(webspace_path)
                                  +strlen(resource_path)+1)*sizeof(char));

  // 1. Concatena caminho  
  sprintf(full_path, "%s%s", webspace_path, resource_path); 
  
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


  free(full_path);
  return 0;
}


int main(int argc, char** argv){
  
  printf("Status %d\n", get_resource(argv[1], argv[2]));
  return 0;
}
