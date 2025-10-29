#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include "servidor_lib.h"

void enviar_erro(int socket, int codigo, const char *mensagem) {
    char resposta[256];
    snprintf(resposta, sizeof(resposta),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %ld\r\n"
             "\r\n"
             "%s",
             codigo, mensagem, strlen(mensagem), mensagem);
    if (write(socket, resposta, strlen(resposta)) < 0)
        printf("Erro ao enviar dados");
}

void enviar_arquivo(int socket, const char *caminho){
    FILE *f = fopen(caminho, "rb");
    if(!f){
        enviar_erro(socket, 404, "Arquivo não encontrado");
        return;
    }
    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);
    char cabecalho[256];
    snprintf(cabecalho, sizeof(cabecalho),
            "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/octet-stream\r\n"
             "Content-Length: %ld\r\n"
             "\r\n",tamanho);
    if (write(socket, cabecalho, strlen(cabecalho)) < 0)
        printf("Erro ao enviar dados");

    char buffer[BUFFER_TAM];
    size_t lidos;
    while((lidos = fread(buffer, 1, sizeof(buffer), f))>0)
        if (write(socket, buffer, lidos) < 0)
            printf("Erro ao enviar dados");
    
    fclose(f);                  
}  

void enviar_listagem(int socket, const char *diretorio){
    DIR *dir = opendir(diretorio);
    if(!dir){
        enviar_erro(socket, 404, "Erro ao abrir o diretório");
        return;
    }
    char resposta[BUFFER_TAM*2];
    strcpy(resposta,
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n\r\n"
           "<!DOCTYPE html><html lang='pt-br'><head>"
           "<meta charset='UTF-8'>"
           "<title>Listagem de Arquivos</title>"
           "<style>"
           "body{font-family:Arial;background:#0C0F42;color:#eee;text-align:center; font-size:20px;}"
           "ul{list-style:none;padding:0;}"
           "li{margin:10px 0;}"
           "a{color:#0ff;text-decoration:none;}"
           "a:hover{text-decoration:underline;}"
           "</style></head><body>"
           "<h1>Arquivos disponíveis</h1><ul>");
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
            strcat(resposta, "<li><a href=\"");
            strcat(resposta, ent->d_name);
            strcat(resposta, "\">");
            strcat(resposta, ent->d_name);
            strcat(resposta, "</a></li>");
        }
    }
    closedir(dir);
    strcat(resposta, "</ul></body></html>");
    if (write(socket, resposta, strlen(resposta)) < 0)
        printf("Erro ao enviar dados");

}