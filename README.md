# Sockets
Application Côté Client
L'application client effectue les tâches suivantes :

Connexion au Socket Serveur :

Le client entre un identifiant (login) et un mot de passe.
Si le mot de passe est incorrect, le client peut réessayer jusqu'à 3 fois avant que le système ne s'arrête.
Si le mot de passe est correct, le client récupère le profil de l'utilisateur (Administrateur ou Invité).
Menu Administrateur :

Une fois connecté en tant qu'administrateur, un menu s'affiche permettant à l'administrateur d'effectuer diverses tâches.
Menu Invité :

Les invités peuvent uniquement consulter et rechercher des contacts.
Transmission de Données :

Le client peut saisir des informations et les envoyer au serveur.
Le serveur reçoit les données et les stocke dans un fichier texte (contacts.txt).
Application Côté Serveur
L'application serveur effectue les tâches suivantes :

Gestion des Connexions :

Le serveur est prêt à recevoir des connexions de clients.
Authentification des Clients :

Le serveur vérifie l'identifiant et le mot de passe des clients.
Si le client est un administrateur, le menu approprié s'affiche.
Si le client est un invité, il peut seulement consulter et rechercher des contacts. Le menu est personnalisé en fonction du profil de l'utilisateur.
Affichage de l'Historique :

Le serveur affiche l'historique instantané des clients.
Gestion des Contacts :

Ajouter, modifier, supprimer et rechercher des contacts.
Le serveur affiche toutes les requêtes reçues et effectue le traitement demandé par le client.

Contributeurs:

Dembele Moussa
Mohamed Amine Ait Jaakike
Saad Barhrouj
Dahman Salma
