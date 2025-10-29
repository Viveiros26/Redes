#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include "servidor_lib.h"



int main(){
    char *diretorio = "./arquivos";
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_TAM] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        printf("Erro ao cria o socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        printf("Erro ao associar ao socket\n");
        close(server_fd);
        return 1;
    }


    if(listen(server_fd, 3) < 0){
        printf("Erro ao escutar");
        close(server_fd);
        return 1;
    }

    printf("Servidor HTTP iniciando na porta %d e servindo o diretorio: %s\n", PORT, diretorio);

    while(1){
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) <0){
            perror("Erro ao aceitar conexão");
            continue;
        }

        memset(buffer, 0, BUFFER_TAM);
        read(new_socket, buffer, BUFFER_TAM -1);
        char metodo[8], caminho[512];
        sscanf(buffer, "%s %s", metodo, caminho);
        if(strcmp(metodo, "GET") != 0){
            enviar_erro(new_socket, 405, "Método Não Permitido");
            close(new_socket);
            continue;
        }

        char caminho_completo[1024];
        snprintf(caminho_completo, sizeof(caminho_completo), "%s%s", diretorio, caminho);

        struct stat st;
        if(stat(caminho_completo, &st) == -1)
            enviar_erro(new_socket, 404, "Arquivo ou diretório não encontrado");
        else if(S_ISDIR(st.st_mode)){
            char index[2048];
            snprintf(index, sizeof(index), "%s/index.html", caminho_completo);
            if(stat(index, &st) != -1)
                enviar_arquivo(new_socket, index);
            else
                enviar_listagem(new_socket, caminho_completo);
        } else
            enviar_arquivo(new_socket, caminho_completo);
        
        close(new_socket);
    }

    close(server_fd);

    return 0;
}