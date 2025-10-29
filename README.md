Descrição
Esse projeto tem como objetivo implementar um simples servidor HTTP e um cliente HTTP na linguagem C, para entender, na prática, o funcionamento desse protocolo. Temos então dois programas principais:

Cliente:
Funciona como um navegador pelo terminal, requisitando para um servidor um arquivo que ele dispõe e baixando esse arquivo e adicionando na pasta downloads.
Para sua execução precisa digitar o comando make compiler, e depois ./cliente http:/host:port/arquivo, exemplo http://localhost:8080/teste.txt. 
Caso não exista a pasta downloads ele a cria automaticamente, então ele cria um socket TCP e conecta ao servidor, envia a requisição e caso retorne o status 200 ele salva o arquivo e adiciona a pasta, caso contrário não salva o arquivo e retorna erro no terminal.

Servidor:
Funciona como um fornecedor de arquivos que estão na pasta ./arquivos, predefinida no código, e então responde as requisições do cliente com esses arquivos que possui.
Para sua execução precisa digitar make run, que se estiver tudo certo aparecerá no terminal "Servidor HTTP iniciando na porta 8080 e servindo o diretorio: ./arquivos" e então ao entrar no navegador é só colocar http://localhost:8080 e estará com os arquivos que o servidor dispõe, ou utilizar o cliente e colocar um arquivo específico. 
O servidor inicialmente cria o socket e conecta na porta, que nesse caso é a 8080, e associa a um IP, nesse caso localhost, em seguida ele escuta as requisições que são feitas e retorna o arquivo ou erros HTTP, caso necessário.

Makefile:
para o servidor exitem os seguintes comandos:
make compiler -> que somente compila os códigos
make clean -> remove os .o e o executável
make run -> faz o make clean, o make compiler e então roda o programa

para o cliente existem os seguintes comandos:
make clean -> remove o .o, o executável e a pasta downloads com os arquivos que estão nela
make compilar -> executa o make clean e depois compila o código

Licença:
Este projeto está licenciado sob a MIT License.
Sinta-se livre para usar, modificar e distribuir.
