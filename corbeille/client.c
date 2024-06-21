#include <stdio.h>
#include<string.h>
#include"Practical.h"
#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5095
#define BUFFER_SIZE 1024
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include<arpa/inet.h>
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
            DieWithUserMessage("WSAStartup()","Failed ");
        
    #endif
    int socketFD= socket(AF_INET,SOCK_STREAM,0);
    if(socketFD==-1)
        DieWithUserMessage("socket()","Initialisation socket echouee");
    struct sockaddr_in socketAddress;
    socketAddress.sin_family= AF_INET;
    socketAddress.sin_port= LISTENING_PORT;
    int inetReturnCode=inet_pton(AF_INET,CONNECTION_HOST,&socketAddress.sin_addr);
    if(inetReturnCode==-1)
        DieWithUserMessage("inet_pton()","Adresse non valide ou non prise en charge");
    int socketAddressLen= sizeof(socketAddress);
    int connexionStatus = connect(socketFD, (struct sockaddr *)&socketAddress,socketAddressLen);
    if (connexionStatus==-1)
        DieWithUserMessage("connect()","Echec de la connexion");
    const char message[]="Bonjour serveur, je suis client.";
    int sentBytes= send(socketFD,message,strlen(message),0);
    if(sentBytes==-1)
        DieWithUserMessage("send()","Echec envoi du message");

    char buffer[BUFFER_SIZE]={0};
    int receivedBytes= recv(socketFD,buffer,BUFFER_SIZE,0);
    if(receivedBytes==-1)
        DieWithUserMessage("recv()"," Erreur lors de la reception");

    printf("Serveur : %s \n",buffer);
    
    #ifdef _WIN32
        closesocket(socketFD);
        WSACleanup();
    #else
        close(socketFD);
    #endif
    return 0;
}