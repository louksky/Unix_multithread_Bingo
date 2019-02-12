#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int indexer = 0;
int games[250];
int count = 0;

void * socketThread(void *arg)
{
  int myindex = indexer;
  int flag = 1;
  int newSocket = *((int *)arg);
  int myround;
  int t = 0;
  int b = 0, c = 0;
  int array1[4], array2[4];
  int temp;
  int theNumber = (rand() % 100) + 2000;

  temp = theNumber;
  myround = count;

  for(int i = 0; i < 4; i++) {
    array1[i] = temp % 10;
    temp /= 10; 
  }

  indexer++;
  if (indexer % 2 != 0) 
    count++;

  printf("-------------\ngame winner is %d \n in %d cou %d \nfor palyer num%d is %d \n--------------- \n", games[myround], indexer, count, myround, theNumber);

  while(recv(newSocket, client_message, 2000, 0) != 0 && flag != 0) {
    pthread_mutex_lock(&lock);
    temp = t = atoi(client_message);
    printf("opd %d  \n",temp);
    printf("-------------\ngame statu is %d", games[myround]);
    for(int i = 0; i < 4; i++){
      array2[i] = temp % 10;
      temp /= 10;
    }

    //GAME LOOP
    c = b = 0;
    for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
    {
      if(i == j && array1[i] == array2[j])
        b++;
      else if(array1[i] == array2[j])
        c++;
    }
    char *message = malloc(sizeof(client_message) + 200);
    if(games[myround] != 0) {
      strcpy(message, "you loss ! ");
      flag = 0;
      printf("losssser  .. hou\n");
      //loss
    }
    else if(b == 4)
    {
      games[myround] = 2000;
      strcpy(message,"you win ! ");
      flag = 0;
      printf("\n.................\ngame statu is %d\n", games[myround] );
    }
    else {
      sprintf(message, "bulls %d cows %d ! ", b, c);
    }
    //printf("yo -%s .. is in the house\n",client_message);

    strcat(message,client_message);
    strcat(message,"\n");
    strcpy(buffer,message);
    free(message);
    pthread_mutex_unlock(&lock);
    sleep(1);
    send(newSocket,buffer,250,0);
  }

  printf("Exit  \n");
  close(newSocket);
  pthread_exit(NULL);
}

int main(){
  printf("yo  .. hou\n");
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  
  for(int j=0;j<250;j++){
    games[j]=0;
  }
  
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7799);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(listen(serverSocket,50)==0)
    printf("on!\n");
  else
    printf("Error\n");

  pthread_t tid[60];
  int i = 0;
  while(1)
  {
    addr_size = sizeof serverStorage;
    newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
    printf("new player !\n");
    if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0 )
      printf("Fail thread\n");
    printf("create thread\n");
    if( i >= 50)
    {
      i = 0;
      while(i < 50)
      {
        pthread_join(tid[i++],NULL);
      }
      i = 0;
    }
  }

  return 0;
}
