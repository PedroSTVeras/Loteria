#include <winsock2.h>
#include <ws2tcpip.h>

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
void modlinapos(const char *arcaposta,int linpost,float valornov){
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
        snprintf(lins[linapost-1],sizeof(lins[linapost-1]),"Valor da aposta: R$%.2f\n",valornov);

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
void modnum(const char *arcaposta,int linpost,int novnums[8]){
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
        snprintf(lins[linapost - 2], sizeof(lins[linapost - 2]), "N�meros apostados: %d, %d, %d, %d, %d, %d, %d, %d\n", novnums[0], novnums[1], novnums[2],novnums[3], novnums[4], novnums[5],novnums[6], novnums[7], novnums[8]);
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

//Faz as apostas do jogador
void aposta(SOCKET cs){
    //Pede nome ao cliente
    printf("\nInsira seu nome: \n");
    char nome[20];
    scanf("%s",nome);
    send(cs, nome, strlen(nome), 0); //Envia Nome
    printf("\n%s",nome);

    //Pede o CPF ao cliente
    char cpf[12];
    printf(", insira seu CPF: \n");
    scanf("%s",&cpf);
    //Verifica se o CPF � v�lido (11 d�gitos), se n for, pede denovo
    while(strlen(cpf)!=11){
        printf("\nInsira um CPF v�lido (11 d�gitios, sem h�fen ou pontos):\n");
        scanf("%s",cpf);
    }
    //Transforma o CPF no formato certo
    char cpfform[15];
    snprintf(cpfform, sizeof(cpfform), "%c%c%c.%c%c%c.%c%c%c-%c%c",cpf[0], cpf[1], cpf[2],cpf[3], cpf[4], cpf[5],cpf[6], cpf[7], cpf[8],cpf[9], cpf[10]);
    printf("\nSeu CPF � %s",cpfform);
    send(cs, cpf, strlen(cpf), 0); //Envia CPF

    //Pede os n�meros da aposta do jogador
    int apostan [8];
    printf("\nInsira 8 n�meros de 1 a 20 (um de cada vez):\n");
    for(int i = 0;i<8;i++){
        while(1){ //Loop at� coletar todos os n�meros
            scanf("%d",&apostan[i]);
            if(apostan[i] >=1 && apostan[i]<=20){
                    if (i < 7)
                        printf("Insira mais %d n�meros (1 a 20):\n", 7-i);
                    break; //Sair do loop ap�s coletar todos n�meros
            }
            else
            {
            printf("Insira uma aposta v�lida:\n");
            }
        }
    }
    //Organiza os n�meros apostados em ordem crescente
    for (int j = 0; j < 7; j++) {
        for (int k = 0; k < 7 - j; k++) {
            if (apostan[k] > apostan[k + 1]) {
                int temp = apostan[k];
                apostan[k] = apostan[k + 1];
                apostan[k + 1] = temp;
            }
        }
    }
    //Escreve n�meros apostados
    char apostaSend[BUFFER_SIZE] = {0};
    printf("\nN�meros apostados: ");
    int aux = 0;
    for(int i = 0; i <7; i++){
        printf("%d, ",apostan[i]);
        apostaSend[aux] = apostan[i] + '0';
        aux++;
        apostaSend[aux] = ',';
        aux++;
        apostaSend[aux] = ' ';
        aux++;
    }
    printf("%d.\n",apostan[7]);
    apostaSend[aux] = apostan[7] + '0';
    send(cs, apostaSend, strlen(apostaSend), 0); //Envia numeros apostados

    //Pergunta a usu�rio quanto dinheiro apostar
    char filename[30];
    int valoraposta = 0;
    printf("Quanto deseja apostar (R$1 a R$1000)?\nR$");
    scanf("%d",&valoraposta);
    //Mensagem de erro se aposta for inv�lida
    while(valoraposta <1 || valoraposta > 1000){
        printf("\nInsira um valor v�lido!\n");
        if(valoraposta<1)printf("Motivo: O valor m�nimo da aposta � R$1\nR$");
        if(valoraposta>1000)printf("Motivo: O valor excede o limite de R$1000\nR$");
        scanf("%f",&valoraposta);
    }

    char money[6] = {0};
    sprintf(money,"%ld", valoraposta);
    send(cs, money, strlen(money), 0); //Envia dinheiro apostado

    printf("Aposta registrada com sucesso\n");
}

//Consultar Aposta
void conaposta(SOCKET cs) {
    char msg[BUFFER_SIZE] = {0};
    char cpf[11];
    char apostaarc[30];
    int apostan [8];
    char linhas[256];
    int opera;

    //Pede CPF do cliente para encontrar aposta
    printf("\nInsira o CPF da aposta que deseja consultar (11 d�gitios, sem h�fen ou pontos):\n");
    scanf("%s", cpf);
    //Verifica se o CPF � v�lido (11 d�gitos), se n for, pede denovo
    while(strlen(cpf)!=11){
        printf("\nInsira um CPF v�lido (11 d�gitios, sem h�fen ou pontos):\n");
        scanf("%s",cpf);
    }
    send(cs, cpf, strlen(cpf), 0); //Envia CPF

    //Espera mensagem do servidor para saber se achou a aposta ou n�o
    recv(cs, msg, sizeof(msg), 0);
    printf("\nServidor: %s\n", msg);

    //Se servidor encontrou arquivo, mostrar ao cliente
    if (strcmp(msg, "Arquivo encontrado!\n")==0){
        recv(cs, msg, sizeof(msg), 0);
        printf("%s", msg);

        //Pergunta o que fazer
        printf("\nO que deseja fazer?\n");
        printf(" 1- Alterar valor da aposta\n 2- Alterar os n�meros\n 3- Remover aposta\n 4- Voltar\n");
        scanf("%d", &opera);

        switch (opera) {
            case 1:{
                //Enviar para o server q qr alterar
                send(cs, "1", 1, 0);
                //Alterar a quantidade de dinheiro apostado
                printf("\nInsira o novo valor para a aposta: R$");
                int novovaloraposta;
                scanf("%d", &novovaloraposta);
                while (novovaloraposta < 1 || novovaloraposta > 1000) {
                    printf("\nInsira um valor v�lido!\n");
                    if(novovaloraposta<1)printf("Motivo: O valor m�nimo da aposta � R$1\nR$");
                    if(novovaloraposta>1000)printf("Motivo: O valor excede o limite de R$1000\nR$");
                    scanf("%d", &novovaloraposta);
                }

                char aux[BUFFER_SIZE] = {0};
                sprintf(aux,"%ld", novovaloraposta);
                send(cs, aux, sizeof(aux), 0);
                recv(cs, aux, sizeof(aux), 0);
                printf("%s",aux);

                break;
            }
            case 2:{
                //Pede os n�meros da aposta do jogador
                printf("\nInsira 8 n�meros de 1 a 20 (um de cada vez):\n");
                for(int i = 0;i<8;i++){
                    while(1){ //Loop at� coletar todos os n�meros
                        scanf("%d",&apostan[i]);
                        if(apostan[i] >=1 && apostan[i]<=20){
                                if (i < 7)
                                    printf("Insira mais %d n�meros (1 a 20):\n", 7-i);
                                break; //Sair do loop ap�s coletar todos n�meros
                        }
                        else
                        {
                        printf("Insira uma aposta v�lida:\n");
                        }
                    }
                }
                //Organiza os n�meros apostados em ordem crescente
                for (int j = 0; j < 7; j++) {
                    for (int k = 0; k < 7 - j; k++) {
                        if (apostan[k] > apostan[k + 1]) {
                            int temp = apostan[k];
                            apostan[k] = apostan[k + 1];
                            apostan[k + 1] = temp;
                        }
                    }
                }
                modnum(apostaarc,4,apostan);
                break;
            }
            case 3:{
                //Apagar aposta/arquivo
                if (remove(apostaarc) == 0)
                    printf("Aposta apagada!\n\n");
                else
                    printf("Erro: Aposta n�o foi apagada!\n\n");
                break;
            }
            case 4:{
                //Voltar
                printf("\n");
                break;
            }
        }
    }
}

int main(){
    setlocale(LC_ALL, "Portuguese");

    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE] = "Ol�, servidor!";

    // Inicializando a Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Falha ao iniciar Winsock." << std::endl;
        return 1;
    }

    // Criando o socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Erro ao criar socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurando o endere�o do servidor
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Conectando ao servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Erro ao conectar ao servidor.");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    else{
        printf("Conectado ao servidor!\n");
    }

    // Enviando mensagem ao servidor
    send(clientSocket, buffer, strlen(buffer), 0);

    // Recebendo resposta inicial do servidor ("Seleciona uma op��o")
    char response[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, response, BUFFER_SIZE, 0);

    //Apresenta as op��es ao cliente
    int opcao = 0;
    while(opcao != 3){

        printf("\n%s\n\n", response);

        //Pede input do cliente (1, 2 ou 3)
        scanf("%d",&opcao);
        switch(opcao){
            //Cria uma nova aposta
            case 1:
            send(clientSocket, "Nova aposta", 11, 0);
            aposta(clientSocket);
            break;
            //Consulta uma aposta
            case 2:
            send(clientSocket, "Consultar aposta", 16, 0);
            conaposta(clientSocket);
            break;
            //Fecha a conex�o
            case 3:
            printf("Desconectando do servidor!\n");
            closesocket(clientSocket);// Fechando o socket
            WSACleanup();
            break;
            //N�mero inv�lido
            default:
                printf("Opera��o inv�lida, insira uma op��o v�lida (1, 2, 3):\n");
                break;
            }
        }

    return 0;
}
