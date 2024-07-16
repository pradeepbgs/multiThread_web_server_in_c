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

struct cache_element
{
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
    struct sockaddr server_addr, client_addr;
}