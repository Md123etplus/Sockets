# Application des Sockets dans la gestion des contacts

## Fonctionnalités

- **Connexion au Serveur :**
  - Le client entre un identifiant (login) et un mot de passe.
  - Gestion de jusqu'à 3 tentatives pour le mot de passe incorrect.
  - Stockage des données des users dans un fichier texte (`compte.txt`) .

- **Menus :**
  - **Administrateur :**
    - Accès complet aux fonctionnalités d'administration.
  - **Invité :**
    - Fonctionnalités restreintes à la consultation et à la recherche de contacts.

- **Transmission de Données :**
  - Saisie d'informations par le client et envoi au serveur.
  - Stockage des données reçues dans un fichier texte (`Contact.txt`).

## Serveur

### Fonctionnalités

- **Gestion des Connexions :**
  - Attente et gestion des connexions entrantes des clients.

- **Authentification :**
  - Vérification des identifiants et des mots de passe des clients.
  - Affichage de menus adaptés selon le type d'utilisateur (administrateur ou invité).

- **Historique des Activités :**
  - Affichage en temps réel des activités des clients.

- **Gestion des Contacts :**
  - Ajout, modification, suppression et recherche de contacts.
  - Traitement des requêtes des clients.

## Contributeurs

- Dembele Moussa
- Mohamed Amine Ait Jaakike
- Saad Barhrouj
- Dahman Salma

  ## Utilisation
(`gcc server2.c -o server -lws2_32`) pour compiler cote serveur
  server
(`gcc client2.c -o client -lws2_32`) pour compiler cote client
  client
  Et la connexion passe sur le meme pc, si on veut le faire entre deux pcs ou plus connecte au meme reseau local on change 
  `socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); en l'adresse ip du host.`
