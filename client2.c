#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> // Pour la fonction close()
#endif

#define LISTENING_PORT 5094
#define BUFFER_SIZE 1024
#define MAX_ATTEMPTS 3

int choix;

typedef struct {
    char nomRue[255];
    char ville[255];
    char pays[255];
} adresse;

typedef struct {
    char nom[255];
    char prenom[255];
    int GSM;
    char email[255];
    adresse adresse;
} contact;

// Fonction pour saisir les informations d'un contact
void saisir(char *message,contact *c) {
    printf("Entrer le nom de contact : ");
    scanf("%s", c->nom);
    sprintf(message, "%s", c->nom);
    strcat(message, "\t");

    printf("Entrer le prenom de contact : ");
    scanf("%s", c->prenom);
    strcat(message, c->prenom);
    strcat(message, "\t");

    printf("Entrer le GSM : ");
    scanf("%d", &c->GSM);
    sprintf(message + strlen(message), "%d", c->GSM); // Ajout du GSM au message

    strcat(message, "\t");

    printf("Entrer l'email : ");
    scanf("%s", c->email);
    strcat(message, c->email);
    strcat(message, "\t");

    printf("\nSaisie de l'adresse \n");
    printf("Veuillez entrer le nom de la rue : ");
    scanf("%s", c->adresse.nomRue);
    strcat(message, c->adresse.nomRue);
    strcat(message, "\t");

    printf("Veuillez entrer la ville du contact : ");
    scanf("%s", c->adresse.ville);
    strcat(message, c->adresse.ville);
    strcat(message, "\t");

    printf("Veuillez entrer le pays du contact : ");
    scanf("%s", c->adresse.pays);
    strcat(message, c->adresse.pays);
}


// Fonction pour afficher le menu en fonction du profil
void displayMenu(char *profil) {
    printf("\n************************** MENU **************************\n");
    if (strcmp(profil, "admin") == 0) {
        printf("1 = Ajouter un contact\n");
        printf("2 = Rechercher un contact\n");
        printf("3 = Supprimer un contact\n");
        printf("4 = Modifier un contact\n");
        printf("5 = Afficher les contacts\n");
    } else {
        printf("1 = Rechercher un contact\n");
        printf("2 = Afficher tous les contacts\n");
    }
    printf("6 = Quitter\n");
    printf("***********************************************************\n");
}

int main(void) {
    // Initialize WinSock on Windows
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "(CLIENT) Echec d'initialisation de Windows\n");
        exit(1);
    }
#endif

    // Create socket
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        fprintf(stderr, "(CLIENT) Echec d'initialisation de socket\n");
        exit(1);
    }

    // Configure socket binding
    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(LISTENING_PORT);
    socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (socketAddress.sin_addr.s_addr == INADDR_NONE) {
        fprintf(stderr, "(CLIENT) Adresse invalide\n");
        exit(1);
    }

    // Connect to the server
    if (connect(socketFD, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1) {
        fprintf(stderr, "(CLIENT) Echec de la connexion au serveur\n");
        perror("connect");
        exit(1);
    }
    printf("(CLIENT) Connexion reussie au serveur\n");
    char login[255], password[255];
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        //system("cls");
        printf("S'il vous plait saisir votre login : ");
        scanf("%s", login);
        printf("S'il vous plait saisir votre mot de passe : ");
        scanf("%s", password);
        // Send login and password to server
        char message[BUFFER_SIZE];
        sprintf(message, "%s:%s", login, password);
        int sentBytes = send(socketFD, message, strlen(message), 0);
        if (sentBytes == -1) {
            fprintf(stderr, "(CLIENT) Echec d'envoi du message au serveur.\n");
            exit(1);
        }

        // Receive response from server
        char profil[BUFFER_SIZE] = {0};
        int receivedBytes = recv(socketFD, profil, BUFFER_SIZE, 0);
        if (receivedBytes == -1) {
            fprintf(stderr, "(CLIENT) Echec de reception du message du serveur.\n");
            exit(1);
        }

        if (strcmp(profil, "retry") == 0) {
            attempts++;
            printf("Les informations d'identification sont incorrectes. Tentatives restantes : %d\n", MAX_ATTEMPTS - attempts);
        } else {
            int GSM;
            char GSMstr[BUFFER_SIZE]={0};
            char retourG[BUFFER_SIZE] = {0};
            char buffer[BUFFER_SIZE] = {0};
            if (strcmp(profil, "admin") == 0) {
                do {
                    displayMenu(profil);
                    printf("Entrer votre choix : ");
                    scanf("%d", &choix);
                    sprintf(buffer, "%d", choix);
                    send(socketFD,buffer,BUFFER_SIZE, 0);
                    switch (choix) {
                        case 1:
                            contact c;
                            saisir(message,&c);
                            printf("\nContact Saisi: %s#%s#%d#%s#%s#%s#%s\n",c.nom,c.prenom,c.GSM,c.email,c.adresse.nomRue,c.adresse.ville,c.adresse.pays);
                            send(socketFD, message, strlen(message), 0);
                            char retour[BUFFER_SIZE]={0};
                            recv(socketFD,retour,BUFFER_SIZE,0);
                            //error handling
                            printf("\n%s\n",retour);
                            break;
                        case 2:
                            printf("Entrez le GSM du Contact a recherche: ");
                            scanf("%d",&GSM);
                            sprintf(GSMstr,"%d",GSM);
                            send(socketFD,GSMstr,BUFFER_SIZE,0);
                            recv(socketFD,retourG,BUFFER_SIZE,0);
                            printf("\n%s\n",retourG);
                            break;
                        case 3:
                            printf("Entrez le GSM du Contact a supprimer: ");
                            scanf("%d",&GSM);
                            sprintf(GSMstr,"%d",GSM);
                            send(socketFD,GSMstr,BUFFER_SIZE,0);
                            recv(socketFD,retourG,BUFFER_SIZE,0);
                            printf("\n%s\n",retourG);
                            break;
                        case 4:
                            printf("Entrez le GSM du Contact a modifier: ");
                            scanf("%d",&GSM);
                            sprintf(GSMstr,"%d",GSM);
                            send(socketFD,GSMstr,BUFFER_SIZE,0);
                            recv(socketFD,retourG,BUFFER_SIZE,0);
                            printf("\n%s\n",retourG);
                            if(strcmp(retourG,"CONTACT INTROUVABLE")!=0){
                                printf("\nEntrez de nouveau le contact avec les modifications -->\n ");
                                contact cM;
                                saisir(message,&cM);
                                printf("\nContact Saisi: %s#%s#%d#%s#%s#%s#%s\n",cM.nom,cM.prenom,cM.GSM,cM.email,cM.adresse.nomRue,cM.adresse.ville,cM.adresse.pays);
                                send(socketFD, message, strlen(message), 0);
                                memset(message,0,sizeof(message));
                                recv(socketFD,message,BUFFER_SIZE,0);
                                printf("\n%s\n",message);
                            }else{
                                printf("\nFIN OPERATON\n");
                            }
                            break;
                        case 5:
                            memset(retourG,0,sizeof(retourG));
                            recv(socketFD,retourG,BUFFER_SIZE,0);
                            printf("\n%s\n",retourG);
                            break;
                        case 6:
                            main();
                            break;
                        default:
                            printf("\nMAUVAISE REQUETE\n");
                        break;
                    }
                } while (choix != 6);
            } else {
                do {
                    displayMenu(profil);
                    printf("Entrer votre choix : ");
                    scanf("%d", &choix);
                    memset(buffer,0,sizeof(buffer));
                    sprintf(buffer, "%d", choix);
                    send(socketFD,buffer,BUFFER_SIZE, 0);
                    switch (choix) {
                        case 1:
                            memset(GSMstr, 0, sizeof(GSMstr));
                            memset(retourG, 0, sizeof(retourG));
                            printf("Entrez le GSM du Contact a recherche: ");
                            scanf("%d",&GSM);
                            sprintf(GSMstr,"%d",GSM);
                            printf("\n-%s\n",GSMstr);
                            send(socketFD,GSMstr,BUFFER_SIZE,0);
                            recv(socketFD,retourG,BUFFER_SIZE,0);
                            printf("\n%s\n",retourG);
                            break;
                        case 2:
                            memset(retourG,0,sizeof(retourG));
                            recv(socketFD,retourG,BUFFER_SIZE,0);
                            printf("\n%s\n",retourG);
                            break;
                        case 6:
                            main();
                            break;
                        default:
                            printf("\nMAUVAISE REQUETE\n");
                        break;
                    }
                } while (choix != 6);
            }
            break;
        }
    }

#ifdef _WIN32
    closesocket(socketFD);
    WSACleanup();
#else
    close(socketFD);
#endif

    return 0;
}
