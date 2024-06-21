#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 2
#define BUFFER_SIZE 1024
#define MAX_ATTEMPTS 3

typedef struct {
    char login[255];
    char password[255];
    char profil[255];
} compte;
typedef struct
{
    char nomRue[255];
    char ville[255];
    char pays[255];
} adresse;
typedef struct
{
    char nom[255];
    char prenom[255];
    int GSM;
    char email[255];
    adresse adresse[255];
} contact;

int checkCredentials(char *login, char *password, char *profil) {
    compte c;
    FILE *f = fopen("compte.txt", "r");
    if (f == NULL) {
        perror("Error opening file");
        exit(1);
    }
    int attempts = 0;
    int isValid = 0;
    while (attempts < MAX_ATTEMPTS && fscanf(f, "%s %s %s", c.login, c.password, c.profil)) {
        if (strcmp(login, c.login) == 0 && strcmp(password, c.password) == 0) {
            isValid = 1;
            strcpy(profil, c.profil);
            break;
        } else {
            attempts++;
            //printf("-Tentative echouee. Tentative rstantes : %d\n", MAX_ATTEMPTS - attempts);
        }
    }
    fclose(f);
    return isValid;
}
void ajouterContact(const char *nomF, char *contact)
{
    FILE *f = fopen(nomF, "a");
    if (f == NULL)
    {
        perror("Error opening file");
        exit(1);
    }
    fprintf(f,"%s\n", contact);
    fclose(f);
}
int rechercherContactParGSM(int gsm, char *retour) {
    contact c;
    FILE *f = fopen("Contact.txt", "r");
    if (f == NULL) {
        perror("Error opening file");
        exit(1);
        return 0;
    }
    while(!feof(f))
    if (fscanf(f, "%s\t%s\t%d\t%s\t%s\t%s\t%s\n", c.nom, c.prenom, &c.GSM, c.email, c.adresse->nomRue, c.adresse->ville, c.adresse->pays)) {
        if (c.GSM == gsm) {
            printf("\nContact trouve : %s %s\n", c.nom, c.prenom);
            sprintf(retour, "\nContact Trouve : %s#%s#%d#%s#%s#%s#%s\n", c.nom, c.prenom, c.GSM, c.email, c.adresse->nomRue, c.adresse->ville, c.adresse->pays);
            fclose(f);
            return 1;
        }
    }
    printf("\nAucun contact trouve avec le numero GSM specifie.\n");
    fclose(f);
    return 0;
}
char* afficherTousLesContacts()
{
    char *retour=(char *)malloc(1024*sizeof(char));
    contact c;
    FILE *f = fopen("Contact.txt", "r");
    if (f == NULL)
    {
        perror("Error opening file");
        exit(1);
    }
    
    sprintf(retour,"\n--- Tous les contacts ---\n");
    while(!feof(f))
    if (fscanf(f, "%s\t%s\t%d\t%s\t%s\t%s\t%s\n", c.nom, c.prenom, &c.GSM, c.email, c.adresse->nomRue, c.adresse->ville, c.adresse->pays))
    {
        sprintf(retour+ strlen(retour),"Nom: %s, Prenom: %s, GSM: %d, Email: %s\n", c.nom, c.prenom, c.GSM, c.email);
    }
    sprintf(retour+ strlen(retour),"--- Fin des contacts ---\n");
    fclose(f);
    return retour;
}
char *supprimerContactParGSM(int gsm)
{
    char *retour;
    FILE *f = fopen("Contact.txt", "r");
    if (f == NULL)
    {
        perror("Error opening file");
        exit(1);
    }
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        perror("Error opening temporary file");
        fclose(f);
        exit(1);
    }
    contact c;
    int found = 0;
    while(!feof(f))
    if (fscanf(f, "%s\t%s\t%d\t%s\t%s\t%s\t%s\n", c.nom, c.prenom, &c.GSM, c.email, c.adresse->nomRue, c.adresse->ville, c.adresse->pays))
    {
        if (c.GSM != gsm)
        {

            fprintf(tempFile,"%s\t%s\t%d\t%s\t%s\t%s\t%s\n",c.nom, c.prenom, c.GSM, c.email, c.adresse->nomRue, c.adresse->ville, c.adresse->pays);
        }
        else
        {
            found = 1;
        }
    }
    fclose(f);
    fclose(tempFile);
    if (!found)
    {
        sprintf(retour,"Aucun contact trouve avec le numero GSM specifie.\n");
    }
    else
    {
        remove("Contact.txt");
        rename("temp.txt", "Contact.txt");
        sprintf(retour,"Le contact : %s#%s#%d#%s#%s#%s#%s  a ete supprime avec succes!\n",c.nom, c.prenom, c.GSM, c.email, c.adresse->nomRue, c.adresse->ville, c.adresse->pays);
        printf("\nLe contact avec le numero GSM specifie a ete supprime avec succes.\n");
    }
    return retour;
}
void gestionMenu(int connectedSocketFD,char choix,char profil[]){
    char buffer[BUFFER_SIZE]={0};
    char GSMstr[BUFFER_SIZE]={0};
    int GSM;
    char retour[BUFFER_SIZE]={0};
    char *contacts;
    if(strcmp(profil,"admin")==0){
        switch(choix){
            case '1':
                char succes[]="Contact Ajoute avec Succes !";
                printf("\n(REQUETE CLIENT) Ajout de contact ...");
                recv(connectedSocketFD,buffer,BUFFER_SIZE,0);
                ajouterContact("Contact.txt",buffer);
                printf("\n%s\n",succes);//pas la peine
                send(connectedSocketFD,succes,strlen(succes),0);
            break;
            case '2':
                memset(GSMstr,0,sizeof(GSMstr));
                printf("\n(REQUETE CLIENT) Recherche de contact...");
                recv(connectedSocketFD,GSMstr,BUFFER_SIZE,0);
                GSM = atoi(GSMstr);
                if(rechercherContactParGSM(GSM,retour)){
                    send(connectedSocketFD,retour,BUFFER_SIZE,0);
                }else{
                    send(connectedSocketFD,"CONTACT INTROUVABLE",BUFFER_SIZE,0);
                }
            break;
            case '3':
                memset(GSMstr,0,sizeof(GSMstr));
                printf("\n(REQUETE CLIENT) Suppression de contact...");
                recv(connectedSocketFD,GSMstr,BUFFER_SIZE,0);
                GSM = atoi(GSMstr);
                char *deleteMsg=supprimerContactParGSM(GSM);
                send(connectedSocketFD,deleteMsg,BUFFER_SIZE,0);
            break;
            case '4':
                memset(retour,0,sizeof(retour));
                memset(GSMstr,0,sizeof(GSMstr));
                memset(buffer,0,sizeof(buffer));
                char success[]="Contact Modifie avec Succes !";
                printf("\n(REQUETE CLIENT) Modification de contact...");
                recv(connectedSocketFD,GSMstr,BUFFER_SIZE,0);
                GSM = atoi(GSMstr);
                if(rechercherContactParGSM(GSM,retour)){
                    send(connectedSocketFD,retour,sizeof(retour),0);
                    supprimerContactParGSM(GSM);//a permuter
                    recv(connectedSocketFD,buffer,BUFFER_SIZE,0);//avec ca
                    ajouterContact("Contact.txt",buffer);
                    printf("\n%s\n",success);//pas la peine
                    send(connectedSocketFD,success,strlen(success),0);
                }else{
                    send(connectedSocketFD,"CONTACT INTROUVABLE",BUFFER_SIZE,0);
                }
            break;
            case '5':
                printf("\n(REQUETE CLIENT) Afficher tous les contacts...");
                contacts = afficherTousLesContacts();
                send(connectedSocketFD, contacts, strlen(contacts), 0);
                printf("\nDone!\n");
                free(contacts);
            break;
            case '6':
            break;
            default:
                printf("\nMAUVAISE REQUETE\n");
            break;
        }
    }else{
        switch(choix){
            case '1':
                printf("\n(REQUETE CLIENT) Recherche de contact...\n");
                recv(connectedSocketFD,GSMstr,BUFFER_SIZE,0);
                GSM = atoi(GSMstr);
                if(rechercherContactParGSM(GSM,retour)){
                    send(connectedSocketFD,retour,BUFFER_SIZE,0);
                }else{
                    send(connectedSocketFD,"CONTACT INTROUVABLE",BUFFER_SIZE,0);
                }
            break;
            case '2':
                printf("\n(REQUETE CLIENT) Affichage de tous les contacts...\n");
                contacts = afficherTousLesContacts();
                send(connectedSocketFD, contacts, strlen(contacts), 0);
                printf("\nDone!\n");
                free(contacts);
            break;
            case '3':
            break;
            default:
                printf("\nMAUVAISE REQUETE\n");
            break;
        }
    }
    
}

void *clientHandler(void *arg) {
    int connectedSocketFD = *((int *)arg);
    int isAuthenticated = 0;
    char login[255], password[255], profil[255];
    int attempts = 0;
    
    while (!isAuthenticated && attempts < MAX_ATTEMPTS) {
        char buffer[BUFFER_SIZE] = {0};
        int receivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);
        printf("\nCLIENT A DEMANDE AUTHENTIFICATION ...\n");
        printf("\n(CLIENT) %s\n",buffer);
        if (receivedBytes == -1) {
            perror("Reception error");
            close(connectedSocketFD);
            pthread_exit(NULL);
        }

        if (receivedBytes == 0) {
            printf("(SERVEUR) Connexion fermee par le client\n");
            break;
        }

        if (sscanf(buffer, "%[^:]:%s", login, password) == 2) {
            isAuthenticated = checkCredentials(login, password, profil);
            if (!isAuthenticated) {
                attempts++;
                printf("Tentative echouee. Tentatives restantes : %d\n", MAX_ATTEMPTS - attempts);
                // Send retry message to client
                const char *retryMessage = "retry";
                int sentBytes = send(connectedSocketFD, retryMessage, strlen(retryMessage), 0);
                if (sentBytes == -1) {
                    perror("(SERVEUR) Erreur lors de l'envoi de la reponse au client");
                    close(connectedSocketFD);
                    pthread_exit(NULL);
                }
                if (sentBytes == 0) {
                    printf("(SERVEUR) Connexion fermee par le client\n");
                    break;
                }
            }else {
                printf("\nAUTHENTIFICATION TROUVE!\n");
                send(connectedSocketFD,profil,strlen(profil),0);
            }
        }
    }

    if (isAuthenticated) {
        printf("\nUser profile: %s\n", profil);
        char buffer[BUFFER_SIZE] = {0};
        while (TRUE) {
            int receivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);
            if (receivedBytes <= 0) {
                break;
            }
            gestionMenu(connectedSocketFD, buffer[0], profil);
        }    
    }

    close(connectedSocketFD);
    pthread_exit(NULL);
}
int main(void) {
    // Initialize WinSock on Windows
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "Failed to initialize Windows\n");
        exit(1);
    }
#endif
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);//descripteur de fichier
    if (socketFD == -1) {
        perror("Socket error");
        exit(1);
    }

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(LISTENING_PORT);
    socketAddress.sin_addr.s_addr = INADDR_ANY ;//addresse du host
    int socketAddressLength = sizeof(socketAddress);
    printf("\nLa socket %d est maintenant ouverte en mode TCP/IP\n",socketFD);
    // Bind socket to IP address and port
    if (bind(socketFD, (struct sockaddr *)&socketAddress, socketAddressLength) == -1) {
        perror("Socket binding error");
        exit(1);
    }

    // Listen for new connections
    if (listen(socketFD, PENDING_QUEUE_MAXLENGTH) == -1) {
        perror("Server error");
        exit(1);
    }
    printf("\nListage du port %d\n",LISTENING_PORT);

    printf("\nEn attente de nouvelles connexions (PORT: %d)...\n",LISTENING_PORT);
    int nombre_connexion =3,i=1;
    while (TRUE) {
        int connectedSocketFD = accept(socketFD, (struct sockaddr *)&socketAddress, (socklen_t *)&socketAddressLength);
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &socketAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
        printf("\nClient se connecte avec la socket :) %d de %s\n",connectedSocketFD,clientIP);
        if (connectedSocketFD == -1) {
            perror("Server error");
            exit(1);
        }
        pthread_t thread;
        if (pthread_create(&thread, NULL, clientHandler, (void *)&connectedSocketFD) != 0) {
            perror("Thread creation error");
            close(connectedSocketFD);
        }
        
        pthread_detach(thread); // Détacher le thread pour libérer automatiquement ses ressources lorsque terminé
        printf("NOMBRE DE CONNEXIONS : %d",i++);
        nombre_connexion--;
        if(nombre_connexion==-1){
            printf("\n\n***************************WARNING****************************\n");
            printf("\n \n \t\t\tSERVEUR A CRASHE\n");
            printf("\n\t\tNOMBRE MAX DE CONNEXION ATTEINT!\n");
            printf("\n \t\t!!APPELEZ L'ADMINISTRATEUR :) !!\n");
            printf("\n**************************************************************\n");
            break;
        }
    }

    // Close the socket
    close(socketFD);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
