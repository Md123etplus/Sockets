#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include"Practical.h"
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
    #include<sys/socket.h>
#endif
int main(int argc, char *argv[]){
    if(argc<3 || argc>4){
        DieWithUserMessage("Parameters","<Server Adress> <Echo word> [<Server Port>]");
    }
    char* serverIP= argv[1];
    char* echoString= argv[2];
    //in_port_t servPort=(argc==4)?atoi(argv[3]):7;

}