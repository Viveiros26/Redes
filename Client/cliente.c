#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 

#define PORT 8080
#define BUFFER_TAM 4096
#define SERVER_IP "127.0.0.1"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Uso: %s http://<host>:<porta>/<arquivo>\n", argv[0]);
        return 1;
    }
    
    char host[256], path[512] = "";
    int port = 80;
    if (sscanf(argv[1], "http://%255[^:/]:%d/%511[^\n]", host, &port, path) < 2) 
        sscanf(argv[1], "http://%255[^/]/%511[^\n]", host, path);

    if(strlen(path) == 0)
        strcpy(path, "");
    
    printf("Conectando em %s:%d e requisitando /%s\n", host, port, path);


    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_TAM] = {0};

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0){
        perror("Endereço inválido");
        exit(EXIT_FAILURE);
    }
    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("Erro ao se conectar");
        exit(EXIT_FAILURE);
    }

    char request[1024];
    snprintf(request, sizeof(request), 
        "GET /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", path, host);
    
    send(sock, request, strlen(request), 0);
    printf("Requisição enviada ao servidor\n\n");

    char *nomeArquivo = strrchr(path, '/');
    if(nomeArquivo)
        nomeArquivo = nomeArquivo+1;
    else
        nomeArquivo = path;

    if(strlen(nomeArquivo)==0)
        nomeArquivo = "index.html";

    FILE *f = fopen(nomeArquivo, "wb");
    if(!f){
        perror("Erro ao criar o arquivo");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    int bytes;
    int cabecalho = 0;
    char *p;
    while((bytes = read(sock, buffer, BUFFER_TAM-1)) > 0){
        buffer[bytes] = '\0';
        if(!cabecalho){
            p = strstr(buffer, "\r\n\r\n");
            if(p){
                cabecalho = 1;
                p += 4;
                fwrite(p, 1, bytes - (p-buffer), f);
            }
        } else
            fwrite(buffer, 1, bytes, f);
    }
    fclose(f);
    printf("Arquivo salvo como: %s\n", nomeArquivo);
    close(sock);
    return 0;
}