# Sherlok13

## Description

**Sherlock13** is a deduction game inspired by the classic Clue/Cluedo. In this game, four players aim to identify the culprit among 13 characters by exchanging information about the figures on their respective character cards. Each player receives three cards, each depicting a character with one of eight possible figures: bulb, notebook, necklace, crown, skull, eye, pipe, and fist. The goal is to deduce the undiscovered card (culprit) by gathering information about the figures held by other players.

## Gameplay

- Each player has three actions during their turn:
  1. Ask all players if they have a specific figure (yes or no response).
  2. Ask a specific player the number of figures they have for a given figure.
  3. Accuse the culprit. Only the accusing player knows the character they accused.

- Players make their choices through a graphical interface where they can view their characters and the figures they know. The interface also allows them to select their actions.

## Networking

The game is played over a network with a central server and four clients, one for each player. Each player has a graphical interface. The central server manages communication between players and game logic.

## Setup and Execution

### Server

To launch the server, execute the following command:

```bash
$ ./server <portno>
```
replace <portno> with the desired port number for the server (e.g. 32000)

### Client

To launch the client, execute the following command/
```bash
$ ./client <IP_server> <portno_server> <IP_client> <portno_client> <nom>
```
Replace <IP_server>, <portno_server>, <IP_client>, <portno_client>, and <nom> with the server's IP address, server port number, client's IP address, client port number, and player's name respectively.
You can find your own IP address with ```ip a``` bash command. Use a different port number for each person.

## Interface

![image](https://github.com/cleman/Sherlok13/assets/98706569/be391a89-06f1-4027-9989-7d2fbc8c72e4)
