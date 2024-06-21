#include <stdio.h>
#include<string.h>
#include"Practical.h"
#define LISTENING_PORT 5095
#define PENDING_QUEUE_MAX_LENGTH 1
#define BUFFER_SIZE 1024
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
    #include<sys/socket.h>
#endif

int main(){
    // if(argc<3 || argc>4){
    //     DieWithUserMessage("Parameters","<Server Adress> <Echo word> [<Server Port>]");
    // }
    // char* serverIP= argv[1];
    // char* echoString= argv[2];
    // in_port_t servPort=(argc==4)?atoi(argv[3]):7;
    #ifdef _WIN32
        WSADATA wsa;
        if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
            DieWithSystemMessage("WSAStartup () Failed");
        
    #endif
    int socketFD = socket(AF_INET, SOCK_STREAM,0);
    if(socketFD==-1)
        DieWithSystemMessage("socket() Failed");
    struct sockaddr_in socketAddress;
    socketAddress.sin_family= AF_INET;
    socketAddress.sin_port= LISTENING_PORT;
    socketAddress.sin_addr.s_addr= INADDR_ANY;
    int socketAddressLen= sizeof(socketAddress);
    int bindReturnCode = bind(socketFD,(struct sockaddr*)&socketAddress,socketAddressLen);
    if(bindReturnCode==-1)
        DieWithSystemMessage("bind() Erreur de liaison socket");
    if(listen(socketFD,PENDING_QUEUE_MAX_LENGTH)==-1)
        DieWithSystemMessage("listen() Echec de l'ecoute");
    puts("En attente de connexions...");

    int connectedSocketFD = accept(socketFD, (struct sockaddr*)&socketAddress,(socklen_t*)&socketAddressLen);
    if(connectedSocketFD==-1)
        DieWithSystemMessage("Connexion echouee");
    puts("");
    puts("Connecte!");

    char buffer[BUFFER_SIZE]={0};
    int receivedBytes= recv(connectedSocketFD,buffer,BUFFER_SIZE,0);
    if(receivedBytes==-1)
        DieWithSystemMessage("recv() Erreur lors de la reception");
    

    printf("Client : %s \n",buffer);
    char message[]="Bonjour, je suis le serveur.";
    int sentBytes= send(connectedSocketFD,message,strlen(message),0);

    if(sentBytes==-1)
        DieWithSystemMessage("Send() erreur d'envoi");
        
    #ifdef _WIN32
        closesocket(connectedSocketFD);
        closesocket(socketFD);
        WSACleanup();
    #else
        close(connectedSocketFD);
        close(socketFD);
    #endif
    return 0;
}