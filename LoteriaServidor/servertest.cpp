#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <locale.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define linapost 4
#define buflin 256
#define PORT 8080
#define BUFFER_SIZE 1024

//Modificar o valor (dinheiro) da aposta
void modlinapos(const char *arcaposta,int linpost,char *valornov){
    FILE *file = fopen(arcaposta,"r");
    if(!file){
        printf("Arquivo inexistente");
        return;
    }
    char lins[linpost][buflin];
    int i = 0;

    while(fgets(lins[i],sizeof(lins[i]),file)&& i < linapost) i++;
    fclose(file);

    if(linpost > 0 && linpost <=i){
        snprintf(lins[linapost-1],sizeof(lins[linapost-1]),"Valor da aposta: R$%s.00\n",valornov);

    }
    else {
        printf("N�mero da linha inv�lido.\n");
        return;
    }
    file = fopen(arcaposta,"w");
        if(!file){
        printf("Arquivo inexistente");
        return;
    }
    for (int j = 0; j < i; j++)fputs(lins[j],file);
    fclose(file);
    printf("Novo valor de aposta salvo!\n\n");
    return;
}

//Modifica os n�meros da aposta
void modnum(const char *arcaposta,int linpost,char *novnums){
    FILE *file = fopen(arcaposta,"r");
    if(!file){
        printf("Arquivo inexistente");
        return;
    }
    char lins[linpost][buflin];
    int i = 0;

    while(fgets(lins[i],sizeof(lins[i]),file)&& i < linapost) i++;
    fclose(file);

    if(linpost > 0 && linpost <=i){
        snprintf(lins[linapost - 2], sizeof(lins[linapost - 2]), "N�meros apostados: %s\n", novnums);
    }
    else {
        printf("N�mero da linha inv�lido.\n");
        return;
    }
    file = fopen(arcaposta,"w");
        if(!file){
        printf("Arquivo inexistente");
        return;
    }
    for (int j = 0; j < i; j++)fputs(lins[j],file);
    fclose(file);
    printf("Novos n�meros de aposta salvos!\n\n");
    return;
}

//Lida com o cliente ap�s se conectar (threads)
void handle_client(SOCKET clientSocket, int id) {
    char msg[BUFFER_SIZE] = {0};
    char apostaarc[30];
    int apostan [8];

    // Recebendo mensagem do cliente
    recv(clientSocket, msg, sizeof(msg), 0);
    printf("Mensagem recebida do cliente %d: %s\n", id, msg);

    // Enviando resposta ao cliente
    const char *reply = "BEM VINDO A APOSTAS TOTALMENTE LEG�TIMAS\nO que deseja fazer?\n 1- Apostar\n 2- Consultar aposta\n 3- Sair";
    send(clientSocket, reply, strlen(reply), 0);

    //LOOP DO CLIENTE
    while(true){
        memset(msg,0,sizeof(msg));//Limpar mensagem
        recv(clientSocket, msg, sizeof(msg), 0);//Receber mensagem do cleinte
        printf("Cliente %d enviou: %s\n", id, msg);//Imprimir mensagem recebida

        //1- FAZER UMA NOVA APOSTA
        if (strcmp(msg, "Nova aposta")==0){
            //Receber mensagem com o nome
            printf("\nEsperando receber o nome...\n");
            char nome[BUFFER_SIZE] = {0};
            recv(clientSocket, nome, sizeof(nome), 0);
            printf("Nome do cliente %d � %s\n", id, nome);

            //Receber mensagem com o CPF
            printf("\nEsperando receber o CPF...\n");
            char cpf[BUFFER_SIZE] = {0};
            recv(clientSocket, cpf, sizeof(cpf), 0);
            //printf("CPF do cliente %d � %s\n", id, cpf);
            //Formata cpf
            char cpfform[15];
            snprintf(cpfform, sizeof(cpfform), "%c%c%c.%c%c%c.%c%c%c-%c%c",cpf[0], cpf[1], cpf[2],cpf[3], cpf[4], cpf[5],cpf[6], cpf[7], cpf[8],cpf[9], cpf[10]);
            printf("CPF do cliente %d � %s\n", id, cpfform);

            //Receber mensagem com os n�meros
            printf("\nEsperando receber os n�meros da aposta...\n");
            char numAp[BUFFER_SIZE] = {0};
            recv(clientSocket, numAp, sizeof(numAp), 0);
            printf("N�meros apostados pelo cliente %d s�o %s\n", id, numAp);

            //Receber mensagem com o dinheiro
            printf("\nEsperando receber o valor apostado...\n");
            char valor[BUFFER_SIZE] = {0};
            recv(clientSocket, valor, sizeof(valor), 0);
            printf("A quantidade de dinheiro apostado pelo cliente %d � %s\n", id, valor);

            //Criar arquivo
            char filename[30];
            snprintf(filename, sizeof(filename), "aposta_%s.txt", cpf);
            FILE *arcaposta = fopen(filename, "w");
            if (arcaposta == NULL) {
                printf("Erro ao abrir o arquivo %s!\n", filename);
                return;
            }
            //Salva os dados no arquivo
            fprintf(arcaposta, "Nome: %s\n", nome);
            fprintf(arcaposta, "CPF: %s\n", cpfform);
            fprintf(arcaposta, "N�meros apostados: %s\n", numAp);
            fprintf(arcaposta, "Valor da aposta: R$%s.00\n",valor);
            fclose(arcaposta);
            printf("Aposta registrada com sucesso em %s\n\n", filename);
        }

        //2 - CONSULTAR APOSTAS
        else if (strcmp(msg, "Consultar aposta")==0){
            //Receber mensagem com o CPF
            printf("\nEsperando receber o CPF para verificar arquivos...\n");
            char cpf[BUFFER_SIZE] = {0};
            recv(clientSocket, cpf, sizeof(cpf), 0);
            printf("CPF do cliente %d � %s\n", id, cpf);

            //Procurando arquivo com o CPF
            snprintf(apostaarc, sizeof(apostaarc), "aposta_%s.txt", cpf);
            FILE *file = fopen(apostaarc, "r");
            //SE ENCONTROU O ARQUIVO:
            if (file != NULL) {
                char linhas[256] = {0};
                printf("\nArquivo encontrado!\n");

                //Enviar ao cliente os dados do arquivo
                send(clientSocket, "Arquivo encontrado!\n", 30, 0);
                while (fgets(linhas, sizeof(linhas), file)) {
                    printf("%s", linhas); //Escreve as linhas do arquivo
                    send(clientSocket, linhas, sizeof(linhas), 0);
                    recv(clientSocket, msg, sizeof(msg), 0);
                }

                //RECEBER ESCOLHA DO CLIENTE SOBRE O QUE FAZER COM A APOSTA CONSULTADA
                char* opcao;
                recv(clientSocket, opcao, sizeof(opcao), 0);
                //1) ALTERAR A QUANTIDADE DE DINHEIRO APOSTADO
                if (strcmp(opcao, "1")==0){
                    char valornov[BUFFER_SIZE] = {0};
                    printf("\nCliente %d quer alterar o valor apostado!\n", id);
                    //Pegar o dinehiro q e vai ser alterado
                    recv(clientSocket, valornov, sizeof(valornov), 0);
                    printf("\nCliente %d quer alterar o valor apostado para %s\n", id, valornov);
                    modlinapos(apostaarc,4,valornov);
                    send(clientSocket, "Dados alterados com sucesso.\n\n", 30, 0);
                }//2) ALTERAR OS N�MEROS QUE FORAM APOSTADOS
                else if (strcmp(opcao, "2")==0){
                    char numnov[BUFFER_SIZE] = {0};
                    printf("\nCliente %d quer alterar os numeros apostados!\n", id);
                    recv(clientSocket, numnov, sizeof(numnov), 0);
                    printf("\nCliente %d quer alterar os numeros apostados para %s\n", id, numnov);
                    modnum(apostaarc,4,numnov);
                    send(clientSocket, "Dados alterados com sucesso.\n\n", 30, 0);
                }//3) DELETAR A APOSTA
                else if (strcmp(opcao, "3")==0){
                    fclose(file);
                    if (remove(apostaarc) == 0){
                        printf("Aposta apagada!\n\n");
                        send(clientSocket, "Aposta apagada!\n\n", 30, 0);
                    }
                    else{
                        printf("Erro: Aposta n�o foi apagada!\n\n");
                        send(clientSocket, "Erro: Aposta n�o foi apagada!\n\n", 30, 0);
                    }
                }
                else{//4) VOLTAR
                    fclose(file);
                }
            }
            //Se n�o encontrou o arquivo
            else{
            printf("CPF n�o foi encontrado.\n\n");
            send(clientSocket, "Seu CPF n�o foi encontrado.\n\n", 30, 0);
            }
        }
        else{//3- CLIENTE DISCONECTOU
        printf("Cliente %d encerrou a conex�o\n", id);
        closesocket(clientSocket);
        return;
        }
    }
}

int main(){
    setlocale(LC_ALL, "Portuguese");

    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];

    // Inicializando a Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha ao iniciar Winsock.");
        return 1;
    }

    // Criando o socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erro ao criar socket.");
        WSACleanup();
        return 1;
    }
    else{
        printf("Socket criado com sucesso!");
    }

    // Configurando o endere�o do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Associando o socket ao endere�o
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Erro ao associar socket.");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Escutando conex�es
    listen(serverSocket, 3);
    printf("\nAguardando conex�es...");

    int c = sizeof(struct sockaddr_in);
    int idCliente = 0;

    // Loop infinito para aceitar m�ltiplos clientes (threads)
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &c);
        if (clientSocket == INVALID_SOCKET) {
            printf("Falha ao aceitar conex�o. C�digo de erro: %s", WSAGetLastError);
            continue;
        }
        printf("\nConex�o aceita!\n");

        // Criar uma nova thread para lidar com o cliente
        std::thread clientThread(handle_client, clientSocket, idCliente++);
        clientThread.detach(); // Permite que a thread rode independentemente
    }

    // Fechando sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
