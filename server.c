#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include<pthread.h>

int MAXSIZE = 24;
int stack[24];
int top = -1;

int isempty() {

   if(top == -1)
      return 1;
   else
      return 0;
}

int isfull() {

   if(top == MAXSIZE)
      return 1;
   else
      return 0;
}

int peek() {
   return stack[top];
}

int pop() {
   int data;

   if(!isempty()) {
      data = stack[top];
      top = top - 1;
      return data;
   } else {
      printf("Could not retrieve data, Stack is empty.\n");
   }
}

int push(int data) {

   if(!isfull()) {
      top = top + 1;
      stack[top] = data;
   } else {
      printf("Could not insert data, Stack is full.\n");
   }
}

char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct playerSockets {
            int player1Socket;
            int player2Socket;
};

void * socketThread(void *arg)
{
  struct playerSockets *struct_ptr = (struct playerSockets*) arg;
  char cards[24];
  for(int i=0;i<24;i++) {
      cards[i] = i;
  }
  char player1CardsVector[24], player2CardsVector[24];
  for(int i=0;i<24;i++) {
    player1CardsVector[i] = '0';
    player2CardsVector[i] = '0';
  }

  /*Deal the cards for both players at the start of the game*/
  time_t t;
  srand((unsigned) time(&t));
  int player1Cards[12];
  for(int i=0;i<12;i++) {
    player1Cards[i] = -1;
  }
  int cardsChosen = 0, chosenCard, unique = 1;
  while(cardsChosen != 12) {
    chosenCard = rand() % 24;
    for(int j=0;j<12;j++) {
        if(player1Cards[j] == chosenCard) {
            unique = 0;
            break;
        }
    }
    if(unique) {
        player1Cards[cardsChosen] = chosenCard;
        player1CardsVector[chosenCard] = '1';
        cardsChosen++;
    } else {
        unique = 1;
    }
  }
  int player2Cards[12];
  int player2CardsCounter = 0;
  unique = 1;
  for (int i=0;i<24;i++) {
    for(int j=0; j<12; j++) {
        if(player1Cards[j] == i) {
            unique = 0;
        }
    }
    if(unique) {
        player2Cards[player2CardsCounter] = i;
        player2CardsVector[i] = '1';
        player2CardsCounter++;
    } else {
        unique = 1;
    }
  }
  char player1CardsToSend[13], player2CardsToSend[13];
  for(int i=0; i<12; i++) {
    player1CardsToSend[i] = player1Cards[i];
    player2CardsToSend[i] = player2Cards[i];
  }

  //inform players which one they are
  player1CardsToSend[12] = 1;
  player2CardsToSend[12] = 2;

  send(struct_ptr->player1Socket, player1CardsToSend, 13, 0);
  send(struct_ptr->player2Socket, player2CardsToSend, 13, 0);

  /*Wait for the player with 9 of hearts*/
  int whoseTurn = 1;
  for(int i=0; i<12;i++) {
    if(player2Cards[i] == 0) {
        whoseTurn = 2;
    }
  }

  char turn;
  if(whoseTurn == 1) {
    turn = '1';
  } else {
    turn = '2';
  }
  char validMove = '0';
  int n;
  char chosenCardsVector[25];
  while(validMove != '1') {
      send(struct_ptr->player1Socket, &turn, 1, 0);
      send(struct_ptr->player2Socket, &turn, 1, 0);
      if(whoseTurn == 1) {
        n=recv(struct_ptr->player1Socket , chosenCardsVector , 25 , 0);
      } else {
        n=recv(struct_ptr->player2Socket , chosenCardsVector , 25 , 0);
      }
      if(strcmp(chosenCardsVector, "1000000000000000000000000") == 0 || strcmp(chosenCardsVector, "1111000000000000000000000") == 0) {
        printf("OK\n");
        push(0);
        if(whoseTurn == 1) player1CardsVector[0] = '0';
        else player2CardsVector[0] = '0';
        if(chosenCardsVector[1] == '1') {
            push(1);
            push(2);
            push(3);
            if(whoseTurn == 1) {
                player1CardsVector[1] = '0';
                player1CardsVector[2] = '0';
                player1CardsVector[3] = '0';
            }
            else {
                player2CardsVector[1] = '0';
                player2CardsVector[2] = '0';
                player2CardsVector[3] = '0';
            }
        }
        validMove = '1';
      } else {
        printf("NOT OK\n");
      }
      if(validMove == '1') {
        if(whoseTurn == 1) {
            send(struct_ptr->player1Socket, &validMove, 1, 0);
        } else {
            send(struct_ptr->player2Socket, &validMove, 1, 0);
        }
      } else {
        if(whoseTurn == 1) {
            send(struct_ptr->player1Socket, &validMove, 1, 0);
        } else {
            send(struct_ptr->player2Socket, &validMove, 1, 0);
        }
      }
  }
    /*Next moves*/
while(player1CardsVector != "00000000000000000000000" || player2CardsVector != "00000000000000000000000") {
    if(whoseTurn == 1) {
        whoseTurn = 2;
        turn = '2';
    } else {
        whoseTurn = 1;
        turn = '1';
    }
    validMove = '0';
    while(validMove == '0') {
        if(whoseTurn == 1) {
            send(struct_ptr->player1Socket, &turn, 1, 0);
            send(struct_ptr->player2Socket, &turn, 1, 0);
            n=recv(struct_ptr->player1Socket , chosenCardsVector , 25 , 0);
        } else {
            send(struct_ptr->player1Socket, &turn, 1, 0);
            send(struct_ptr->player2Socket, &turn, 1, 0);
            n=recv(struct_ptr->player2Socket , chosenCardsVector , 25 , 0);
        }
        int chosenCardsIndices[4];
        for(int i=0;i<4;i++) {
            chosenCardsIndices[i] = -1;
        }
        int chosenCardsCount = 0;
        validMove = '1';
        for(int i=0; i<24; i++) {
            if(chosenCardsVector[i] == '1') {
                if(chosenCardsCount == 5) {
                    validMove = '0';
                }
                chosenCardsIndices[chosenCardsCount] = i;
                chosenCardsCount++;
            }
        }
        if(validMove == '1') {
            if((peek() == 0 && strcmp(chosenCardsVector, "0111000000000000000000000") == 0) ||
                ((chosenCardsCount == 1) && (peek() / 4 <= chosenCardsIndices[0] / 4)) ||
                ((chosenCardsCount == 4) && (chosenCardsIndices[0] + 3 == chosenCardsIndices[3]) && (chosenCardsIndices[0] % 4 == 0) &&
                    (peek() / 4 < chosenCardsIndices[0] / 4)
                )
            ) {
                validMove = '1';
                printf("OK\n");
                for(int i=0; i<chosenCardsCount; i++) {
                    push(chosenCardsIndices[i]);
                    if(whoseTurn == 1) {
                        player1CardsVector[chosenCardsIndices[i]] = '0';
                        printf("player1: %s\n", player1CardsVector);
                    } else {
                        player2CardsVector[chosenCardsIndices[i]] = '0';
                        printf("player1: %s\n", player2CardsVector);
                    }
                }
            } else {
                printf("NOT OK\n");
                validMove = '0';
            }
        }
        if(whoseTurn == 1) {
            send(struct_ptr->player1Socket, &validMove, 1, 0);
        } else {
            send(struct_ptr->player2Socket, &validMove, 1, 0);
        }
    }
}
  printf("Exit socketThread \n");

  pthread_exit(NULL);
}

int main() {

    struct sockaddr_in address;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&address, 0, sizeof address);

    int opt=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(1100);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof address) == -1) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    pthread_t thread_id;
    struct playerSockets playerSockets;
    for(;;) {
        addr_size = sizeof serverStorage;
//        int newSocket = accept(sockfd, (struct sockaddr *) &serverStorage, &addr_size);
//        int newSocket2 = accept(sockfd, (struct sockaddr *) &serverStorage, &addr_size);
        struct playerSockets;
        playerSockets.player1Socket = accept(sockfd, (struct sockaddr *) &serverStorage, &addr_size);
        playerSockets.player2Socket = accept(sockfd, (struct sockaddr *) &serverStorage, &addr_size);
        //printf("new socket: %d\n", newSocket);
        if(pthread_create(&thread_id, NULL, socketThread, &playerSockets) != 0) {
            printf("Failed to create thread\n");
        }

        pthread_detach(thread_id);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}