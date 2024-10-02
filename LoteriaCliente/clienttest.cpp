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
        printf("Número da linha inválido.\n");
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

//Modifica os números da aposta
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
        snprintf(lins[linapost - 2], sizeof(lins[linapost - 2]), "Números apostados: %d, %d, %d, %d, %d, %d, %d, %d\n", novnums[0], novnums[1], novnums[2],novnums[3], novnums[4], novnums[5],novnums[6], novnums[7], novnums[8]);
    }
    else {
        printf("Número da linha inválido.\n");
        return;
    }
    file = fopen(arcaposta,"w");
        if(!file){
        printf("Arquivo inexistente");
        return;
    }
    for (int j = 0; j < i; j++)fputs(lins[j],file);
    fclose(file);
    printf("Novos números de aposta salvos!\n\n");
    return;
}

//Faz as apostas do jogador
void aposta(int idap){
    //Pede nome ao cliente
    printf("\nInsira seu nome: \n");
    char nome[20];
    scanf("%s",nome);
    printf("\n%s",nome);
    //Pede o CPF ao cliente
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

    //Pede os números da aposta do jogador
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
    printf("\nNúmeros apostados: ");
    for(int i = 0; i <7; i++)printf("%d, ",apostan[i]);
    printf("%d.\n",apostan[7]);
    //Pergunta a usuário quanto dinheiro apostar
    char filename[30];
    float valoraposta = 0;
    printf("Quanto deseja apostar (R$1 a R$1000)?\nR$");
    scanf("%f",&valoraposta);
    //Mensagem de erro se aposta for inválida
    while(valoraposta <1 || valoraposta > 1000){
        printf("\nInsira um valor válido!\n");
        if(valoraposta<1)printf("Motivo: O valor mínimo da aposta é R$1\nR$");
        if(valoraposta>1000)printf("Motivo: O valor excede o limite de R$1000\nR$");
        scanf("%f",&valoraposta);
    }
    //Cria arquivo
    snprintf(filename, sizeof(filename), "aposta_%s.txt", cpf);
    FILE *arcaposta = fopen(filename, "w");
    if (arcaposta == NULL) {
        printf("Erro ao abrir o arquivo %s!\n", filename);
        return;
    }
    //Salva os dados no arquivo
    fprintf(arcaposta, "Nome: %s\n", nome);
    fprintf(arcaposta, "CPF: %s\n", cpfform);
    fprintf(arcaposta, "Números apostados: ");
    for (int i = 0; i < 8; i++) {
        fprintf(arcaposta, "%d", apostan[i]);
        if (i < 7) {
            fprintf(arcaposta, ", ");
        }
    }
    fprintf(arcaposta, "\n");
    fprintf(arcaposta, "Valor da aposta: R$%.2f\n",valoraposta);
    fclose(arcaposta);
    printf("Aposta registrada com sucesso em %s\n\n", filename);
}

//Consultar Aposta
void conaposta() {
    char cpf[11];
    char apostaarc[30];
    int apostan [8];
    char linhas[256];
    int opera;

    printf("\nInsira o CPF da aposta que deseja consultar (11 dígitios, sem hífen ou pontos):\n");
    scanf("%s", cpf);

    //Procurando arquivo com o CPF
    snprintf(apostaarc, sizeof(apostaarc), "aposta_%s.txt", cpf);
    FILE *file = fopen(apostaarc, "r");
    if (file != NULL) {
        printf("\nArquivo encontrado!\n");
        while (fgets(linhas, sizeof(linhas), file)) {
            printf("%s", linhas); //Escreve as linhas do arquivo
        }

        //Pergunta o que fazer
        printf("\nO que deseja fazer?\n");
        printf(" 1- Alterar valor da aposta\n 2- Alterar os números\n 3- Remover aposta\n 4- Voltar\n");
        scanf("%d", &opera);

        switch (opera) {
            case 1:
                //Alterar a quantidade de dinheiro apostado
                printf("\nInsira o novo valor para a aposta: R$");
                float novovaloraposta;
                scanf("%f", &novovaloraposta);
                while (novovaloraposta < 1 || novovaloraposta > 1000) {
                    printf("\nInsira um valor válido!\n");
                    if(novovaloraposta<1)printf("Motivo: O valor mínimo da aposta é R$1\nR$");
                    if(novovaloraposta>1000)printf("Motivo: O valor excede o limite de R$1000\nR$");
                    scanf("%f", &novovaloraposta);
                }
                modlinapos(apostaarc,4,novovaloraposta);
                fclose(file);
                break;

            case 2:
                //Pede os números da aposta do jogador
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
                modnum(apostaarc,4,apostan);
                fclose(file);
                break;

            case 3:
                //Apagar aposta/arquivo
                fclose(file);
                if (remove(apostaarc) == 0)
                    printf("Aposta apagada!\n\n");
                else
                    printf("Erro: Aposta não foi apagada!\n\n");
                break;

            case 4:
                //Voltar
                printf("\n");
                break;
        }
    }
    else{
        printf("CPF não foi encontrado.\n\n");
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
        std::cerr << "Erro ao conectar ao servidor." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Enviando mensagem ao servidor
    send(clientSocket, buffer, strlen(buffer), 0);

    // Recebendo resposta do servidor (Mensagem recebida com sucesso!)
    char response[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, response, BUFFER_SIZE, 0);
    response[bytesReceived] = '\0'; // Null-terminando a string
    std::cout << "Resposta do servidor: " << response << std::endl;



    // Fechando o socket
    closesocket(clientSocket);
    WSACleanup();

    int opera = 0;
    int idap = 1;
    while(opera != 3){
        printf("BEM VINDO A APOSTAS TOTALMENTE LEGÍTIMAS\nO que deseja fazer?\n 1- Apostar\n 2- Consultar aposta\n 3- Sair \n");
        scanf("%d",&opera);
        switch(opera){
            case 1:
            aposta(idap++);
            //printf("Aposta concluída, o que deseja fazer?\n 1- Apostar\n 2- Consultar aposta\n 3- Sair \n");
            break;
            case 2:
            conaposta();
            break;
            case 3:
            opera = 3;
            break;
            default:
                while(opera<1 || opera>3){
                    printf("\nOperação inválida, insira uma opção válida (1, 2, 3):\n");
                    break;
                }
        }
    }
    return 0;
}
