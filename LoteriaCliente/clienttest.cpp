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

//FAZER APOSTAS
void aposta(SOCKET cs){
    //NOME DO CLIENTE
    printf("\nInsira seu nome: \n");
    char nome[20];
    scanf("%s",nome);
    send(cs, nome, strlen(nome), 0); //Envia Nome
    printf("\n%s",nome);

    //CPF DO CLIENTE
    char cpf[12];
    printf(", insira seu CPF: \n");
    scanf("%s",&cpf);
    //Verifica se o CPF é válido (11 dígitos), se n for, pede denovo
    while(strlen(cpf)!=11){
        printf("\nInsira um CPF válido (11 dígitios, sem hífen ou pontos):\n");
        scanf("%s",cpf);
    }
    //Transforma o CPF no formato certo
    char cpfform[15];
    snprintf(cpfform, sizeof(cpfform), "%c%c%c.%c%c%c.%c%c%c-%c%c",cpf[0], cpf[1], cpf[2],cpf[3], cpf[4], cpf[5],cpf[6], cpf[7], cpf[8],cpf[9], cpf[10]);
    printf("\nSeu CPF é %s",cpfform);
    send(cs, cpf, strlen(cpf), 0); //Envia CPF

    //NÚMEROS Q SERÃO APOSTADOS
    int apostan [8];
    printf("\nInsira 8 números de 1 a 20 (um de cada vez):\n");
    for(int i = 0;i<8;i++){
        while(1){ //Loop até coletar todos os números
            scanf("%d",&apostan[i]);
            if(apostan[i] >=1 && apostan[i]<=20){
                    if (i < 7)
                        printf("Insira mais %d números (1 a 20):\n", 7-i);
                    break; //Sair do loop após coletar todos números
            }
            else
            {
            printf("Insira uma aposta válida:\n");
            }
        }
    }
    //Organiza os números apostados em ordem crescente
    for (int j = 0; j < 7; j++) {
        for (int k = 0; k < 7 - j; k++) {
            if (apostan[k] > apostan[k + 1]) {
                int temp = apostan[k];
                apostan[k] = apostan[k + 1];
                apostan[k + 1] = temp;
            }
        }
    }
    //Escreve números apostados
    char apostaSend[BUFFER_SIZE] = {0};
    printf("\nNúmeros apostados: ");
    int aux = 0;
    for(int i = 0; i <7; i++){
        printf("%d, ",apostan[i]);
    }
    snprintf(apostaSend, sizeof(apostaSend), "%d, %d, %d, %d, %d, %d, %d, %d",apostan[0], apostan[1], apostan[2],apostan[3], apostan[4], apostan[5],apostan[6], apostan[7]);
    printf("%d.\n",apostan[7]);
    send(cs, apostaSend, strlen(apostaSend), 0); //Envia numeros apostados

    //DINHEIRO A SER APOSTADO
    char filename[30];
    int valoraposta = 0;
    printf("Quanto deseja apostar (R$1 a R$1000)?\nR$");
    scanf("%d",&valoraposta);
    //Mensagem de erro se aposta for inválida
    while(valoraposta <1 || valoraposta > 1000){
        printf("\nInsira um valor válido!\n");
        if(valoraposta<1)printf("Motivo: O valor mínimo da aposta é R$1\nR$");
        if(valoraposta>1000)printf("Motivo: O valor excede o limite de R$1000\nR$");
        scanf("%d",&valoraposta);
    }
    char money[6] = {0};
    sprintf(money,"%ld", valoraposta);
    send(cs, money, strlen(money), 0); //Envia dinheiro apostado

    //Aposta registrada
    printf("Aposta registrada com sucesso\n");
}

//CONSULTAR APOSTAS
void conaposta(SOCKET cs) {
    char msg[BUFFER_SIZE] = {0};
    char cpf[11];
    char apostaarc[30];
    int apostan [8];
    char linhas[256];
    int opera;

    //Pede CPF do cliente para encontrar aposta
    printf("\nInsira o CPF da aposta que deseja consultar (11 dígitios, sem hífen ou pontos):\n");
    scanf("%s", cpf);
    //Verifica se o CPF é válido (11 dígitos), se n for, pede denovo
    while(strlen(cpf)!=11){
        printf("\nInsira um CPF válido (11 dígitios, sem hífen ou pontos):\n");
        scanf("%s",cpf);
    }
    send(cs, cpf, strlen(cpf), 0); //Envia CPF

    //Espera mensagem do servidor para saber se achou a aposta ou não
    recv(cs, msg, sizeof(msg), 0);
    printf("\nServidor: %s\n", msg);

    //Se o servidor encontrou arquivo, mostrar os dados da aposta ao cliente
    if (strcmp(msg, "Arquivo encontrado!\n")==0){
        for(int i = 0; i< 4;i++){
            recv(cs, msg, sizeof(msg), 0);
            printf("%s", msg);
            send(cs, "Received!", 20, 0);
        }

        //Pergunta o que fazer com os dados
        printf("\nO que deseja fazer?\n");
        printf(" 1- Alterar valor da aposta\n 2- Alterar os números\n 3- Remover aposta\n 4- Voltar\n");
        scanf("%d", &opera);

        switch (opera) {
            case 1:{//ALTERAR A QUANTIDADE DE DINHEIRO ALTERADO
                //Enviar para o server q qr alterar
                send(cs, "1", 1, 0);
                printf("\nInsira o novo valor para a aposta: R$");
                int novovaloraposta;
                scanf("%d", &novovaloraposta);
                //checar se a aposta é válida
                while (novovaloraposta < 1 || novovaloraposta > 1000) {
                    printf("\nInsira um valor válido!\n");
                    if(novovaloraposta<1)printf("Motivo: O valor mínimo da aposta é R$1\nR$");
                    if(novovaloraposta>1000)printf("Motivo: O valor excede o limite de R$1000\nR$");
                    scanf("%d", &novovaloraposta);
                }
                //Enviar ao servidor novo valor da aposta
                char aux[BUFFER_SIZE] = {0};
                sprintf(aux,"%ld", novovaloraposta);
                send(cs, aux, sizeof(aux), 0);
                recv(cs, aux, sizeof(aux), 0);
                printf("%s",aux);// Recebe se a troca teve sucesso

                break;
            }
            case 2:{
                //ALTERAR OS NÚMEROS APOSTADOS
                send(cs, "2", 1, 0);
                printf("\nInsira 8 números de 1 a 20 (um de cada vez):\n");
                for(int i = 0;i<8;i++){
                    while(1){ //Loop até coletar todos os números
                        scanf("%d",&apostan[i]);
                        if(apostan[i] >=1 && apostan[i]<=20){
                                if (i < 7)
                                    printf("Insira mais %d números (1 a 20):\n", 7-i);
                                break; //Sair do loop após coletar todos números
                        }
                        else
                        {
                        printf("Insira uma aposta válida:\n");
                        }
                    }
                }
                //Organiza os números apostados em ordem crescente
                for (int j = 0; j < 7; j++) {
                    for (int k = 0; k < 7 - j; k++) {
                        if (apostan[k] > apostan[k + 1]) {
                            int temp = apostan[k];
                            apostan[k] = apostan[k + 1];
                            apostan[k + 1] = temp;
                        }
                    }
                }
                //Escreve números apostados em um char
                char apostaSend[BUFFER_SIZE] = {0};
                printf("\nNúmeros apostados: ");
                int aux = 0;
                for(int i = 0; i <7; i++){
                    printf("%d, ",apostan[i]);
                }
                printf("%d.\n",apostan[7]);

                snprintf(apostaSend, sizeof(apostaSend), "%d, %d, %d, %d, %d, %d, %d, %d",apostan[0], apostan[1], apostan[2],apostan[3], apostan[4], apostan[5],apostan[6], apostan[7]);

                send(cs, apostaSend, strlen(apostaSend), 0); //Envia numeros apostados
                recv(cs, apostaSend, sizeof(apostaSend), 0); //Recebe se a troca teve sucesso
                printf("%s",apostaSend);

                break;
            }
            case 3:{
                //APAGAR APOSTA/ARQUIVO
                send(cs, "3", 1, 0);
                char aux[BUFFER_SIZE] = {0};
                recv(cs, aux, sizeof(aux), 0);//recebe se foi deletado com sucesso
                printf("%s",aux);
                break;
            }
            case 4:{
                //VOLTAR
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
    char buffer[BUFFER_SIZE] = "Olá, servidor!";

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

    // Configurando o endereço do servidor
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

    // Recebendo resposta inicial do servidor ("Seleciona uma opção")
    char response[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, response, BUFFER_SIZE, 0);

    //Apresenta as opções ao cliente (loop)
    int opcao = 0;
    while(opcao != 3){

        printf("\n%s\n\n", response);

        //Pede input do cliente (1, 2 ou 3)
        scanf("%d",&opcao);
        switch(opcao){
            //1 - CRIAR NOVA APOSTA
            case 1:
            send(clientSocket, "Nova aposta", 11, 0);
            aposta(clientSocket);
            break;
            //2 - CONSULTAR APOSTA JÁ FEITA
            case 2:
            send(clientSocket, "Consultar aposta", 16, 0);
            conaposta(clientSocket);
            break;
            //3 - ENCERRAR CONEXÃO E FECHAR CLIENTE
            case 3:
            printf("Desconectando do servidor!\n");
            closesocket(clientSocket);// Fechando o socket
            WSACleanup();
            break;
            //Número inválido, tente de novo
            default:
                printf("Operação inválida, insira uma opção válida (1, 2, 3):\n");
                break;
            }
        }

    return 0;
}
