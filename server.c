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
//  for(int i=0;i<13;i++) {
//    printf("%ld ", player1CardsToSend[i]);
//  }
//  printf('\n');
  send(struct_ptr->player1Socket, player1CardsToSend, 13, 0);
  send(struct_ptr->player2Socket, player2CardsToSend, 13, 0);


  /*Wait for the player with 9 of hearts*/
  int whoseTurn = 1;
  for(int i=0; i<12;i++) {
    if(player2Cards[i] == 0) {
        whoseTurn = 2;
    }
  }
  printf("Player %d:\n", whoseTurn);
  char turn;
  if(whoseTurn == 1) {
    turn = '1';
  } else {
    turn = '2';
  }
  send(struct_ptr->player1Socket, &turn, 1, 0);
  send(struct_ptr->player2Socket, &turn, 1, 0);
  int n;
  char chosenCardsVector[24];
  if(whoseTurn == 1) {
    n=recv(struct_ptr->player1Socket , chosenCardsVector , 24 , 0);
  } else {
    n=recv(struct_ptr->player2Socket , chosenCardsVector , 24 , 0);
  }
  for (int i=0;i<24; i++) {
  printf("%d %d\n", i, chosenCardsVector[i]);
  }
  char validMove = '0';
  if(strcmp(chosenCardsVector, "100000000000000000000000") == 0 || strcmp(chosenCardsVector, "111100000000000000000000") == 0) {
    printf("OK\n");
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




  //printf("my struct: %d\n", struct_ptr->newSocket);
  //printf("my struct: %d\n", struct_ptr->newSocket2);
//  int n;
//  for(;;){
//    n=recv(struct_ptr->player1Socket , client_message , 2000 , 0);
//        if(n<1){
//            break;
//        }
//
//    char *message = malloc(sizeof(client_message));
//    strcpy(message,client_message);
//    sleep(1);
//    send(struct_ptr->player2Socket,message,11,0);
//    memset(&client_message, 0, sizeof (client_message));
//
//    }
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