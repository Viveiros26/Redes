#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>


#define PORT 8080
#define BUFFER_TAM 4096

void enviar_erro(int socket, int codigo, const char *mensagem) {
    char resposta[256];
    snprintf(resposta, sizeof(resposta),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %ld\r\n"
             "\r\n"
             "%s",
             codigo, mensagem, strlen(mensagem), mensagem);
    write(socket, resposta, strlen(resposta));
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
    write(socket, cabecalho, strlen(cabecalho));

    char buffer[BUFFER_TAM];
    size_t lidos;
    while((lidos = fread(buffer, 1, sizeof(buffer), f))>0)
        write(socket, buffer, lidos);
    
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
           "body{font-family:Arial;background:#0C0F42;color:#;text-align:center; font-size:20px;}"
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
    write(socket, resposta, strlen(resposta));

}

int main(){
    char *diretorio = "./arquivos";
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_TAM] = {0};
    const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Erro ao cria o socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Erro ao associar ao socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    if(listen(server_fd, 3) < 0){
        perror("Erro ao escutar");
        close(server_fd);
        exit(EXIT_FAILURE);
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