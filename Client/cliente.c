#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <netdb.h>

#define BUFFER_TAM 4096


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Uso: %s http://<host>:<porta>/<arquivo>\n", argv[0]);
        return 1;
    }
    
    char host[256], path[512] = "";
    int port = 8080;
    if (sscanf(argv[1], "http://%255[^:/]:%d/%511[^\n]", host, &port, path) < 2) 
        sscanf(argv[1], "http://%255[^/]/%511[^\n]", host, path);

    if(strlen(path) == 0)
        strcpy(path, "");
    
    printf("Conectando em %s:%d e requisitando /%s\n", host, port, path);

    struct addrinfo hints, *res, *p;
    int sock = 0;
    char port_str[10];
    snprintf(port_str, sizeof(port_str), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, port_str, &hints, &res);
    if(status != 0){
        printf("Erro ao resolver host (%s): %s\n", host, gai_strerror(status));
        return 1;
    }

    for(p = res; p != NULL; p = p->ai_next){
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sock == -1)
            continue;
        if(connect(sock, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(sock);
    }

    if(p == NULL){
        printf("Não foi possível conectar ao servidor %s:%d\n", host, port);
        freeaddrinfo(res);
        return 1;
    }

    freeaddrinfo(res);


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

    struct stat st;
    if(stat("downloads", &st) == -1)
        mkdir("downloads", 0755);

    char caminhoCompleto[1024];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "downloads/%s", nomeArquivo);
    FILE *f = fopen(caminhoCompleto, "wb");
    if(!f){
        perror("Erro ao criar o arquivo");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    int bytes;
    int cabecalho = 0;
    char *pbuffer;
    char buffer[BUFFER_TAM];
    while((bytes = read(sock, buffer, BUFFER_TAM-1)) > 0){
        buffer[bytes] = '\0';
        if(!cabecalho){
            pbuffer = strstr(buffer, "\r\n\r\n");
            if(pbuffer){
                cabecalho = 1;
                pbuffer += 4;
                fwrite(pbuffer, 1, bytes - (pbuffer-buffer), f);
            }
        } else
            fwrite(buffer, 1, bytes, f);
    }
    fclose(f);
    close(sock);
    printf("Arquivo salvo como: %s\n", nomeArquivo);
    return 0;
}