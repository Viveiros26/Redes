#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>


#define PORT 8080
#define BUFFER_TAM 4096

void enviar_erro(int socket, int codigo, const char *mensagem);
void enviar_arquivo(int socket, const char *caminho);
void enviar_listagem(int socket, const char *diretorio);