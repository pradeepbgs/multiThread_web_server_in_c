#include <stdio.h>
#include "proxy_parse.h"
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_CLIENT 10

typedef struct cache_element cache_element;

struct cache_element{
   char* data;
   int len;
   char* url;
   time_t lru_time_track;
   cache_element* next;
};

cache_element* find(char* url);
int add_cache_element(char* data, int len, char * url);
void remove_cache_element();

int port = 8080;
int proxy_socketId;
pthread_t tid[MAX_CLIENT];
sem_t semaphore;
pthread_mutex_t lock;

cache_element* head;
int cache_size;


int main (int argc, char* argv[]) {
    int client_socketid, client_len;
    struct sockaddr_in server_addr, client_addr;
    sem_init(&semaphore,0,MAX_CLIENT);

    pthread_mutex_init(&lock,NULL);
    if (argv == 2){
        port = atoi(argv[1]);
    } else{
        printf("Too few arguments\n");
        exit(1);
    }
    
    printf("Starting proxy server at port: %d\n",port);

    // we will 1 Main proxy server here where every user will request
    // then after we will create a thread for every user ;
    proxy_socketId = socket(AF_INET,SOCK_STREAM,0);
    if (proxy_socketId<0){
        perror("failed to create a socketid");
    }

    int reuse = 1;
    if (setsockopt(proxy_socketId,SOL_SOCKET,SO_REUSEADDR, (const char*)&reuse,sizeof(reuse))<0){
        perror("setSockOpt failded\n");
    }
    
    bzero((char*)&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(proxy_socketId,(struct sockaddr*)&server_addr,sizeof(server_addr)<0)){
        perror("Port is not available\n");
        exit(1);
    }

    printf("binding on port: %d ",port);
    int listen_status = listen(proxy_socketId,MAX_CLIENT);
    if (listen_status<0){
        perror("Error in listening");
    }

    int i =0;
    int Connected_socketId[MAX_CLIENT];
    while(1){
        bzero((char*)&client_addr,sizeof(client_addr));
        client_len = sizeof(client_addr);
        client_socketid = accept(proxy_socketId,(struct sockaddr*)&client_addr,(socklen_t*)&client_len);
        if (client_socketid < 0) {
            perror("Not able to connect\n");
            exit(1);
        } else {
            Connected_socketId[i] = client_socketid;
        }
            struct sockaddr_in * client_pt = (struct sockadrr_in *)&client_addr;
            struct in_addr ip_addr = client_pt->sin_addr;
            char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN);
            printf("Client is connected with port number %d and ip address is %s\n",
              ntohs(client_addr.sin_port),str);
            
            
    }

}
