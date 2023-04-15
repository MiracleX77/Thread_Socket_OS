
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> 
#include <string.h>


#define SERVER_PORT_1 12446
#define SERVER_PORT_2 22446
#define MAX_MSG 100

int main(int argc,char *argv[]){

    int sd_1,sd_2,rc,i,n;
    struct sockaddr_in localAddr1,localAddr2,servAddr1,servAddr2;
    struct hostent *h;
    char recvBuff_1[1024];
    char recvBuff_2[1024];

    
    
    if(argc < 2) {
        printf("usage: %s <server> <data1> <data2> ... <dataN>\n",argv[0]);
        exit(1);
    }

    h = gethostbyname(argv[1]);
    if(h==NULL){
        printf("%s: Unknown Host '%s' \n",argv[0],argv[1]);
        exit(1);
    }

    servAddr1.sin_family = h->h_addrtype;
    memcpy((char *) &servAddr1.sin_addr.s_addr, h->h_addr_list[0],h->h_length);
    servAddr1.sin_port = htons(SERVER_PORT_1);

    servAddr2.sin_family = h->h_addrtype;
    memcpy((char *) &servAddr2.sin_addr.s_addr, h->h_addr_list[0],h->h_length);
    servAddr2.sin_port = htons(SERVER_PORT_2);

    sd_1 = socket(AF_INET,SOCK_STREAM,0);
    if(sd_1<0){
        perror("Cannot open Socket");
        exit(1);
    }
    sd_2 = socket(AF_INET,SOCK_STREAM,0);
    if(sd_2<0){
        perror("Cannot open Socket");
        exit(1);
    }

    localAddr1.sin_family = AF_INET;
    localAddr1.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr1.sin_port = htons(0);
    rc = bind(sd_1,(struct sockaddr *) &localAddr1,sizeof(localAddr1));
    if(rc<0){
        printf("%s: cannot bind port TCP %u\n",argv[0],SERVER_PORT_1);
        perror("ERR");
        exit(1);
    }
    localAddr2.sin_family = AF_INET;
    localAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr2.sin_port = htons(0);
    rc = bind(sd_2,(struct sockaddr *) &localAddr2,sizeof(localAddr2));
    if(rc<0){
        printf("%s: cannot bind port TCP %u\n",argv[0],SERVER_PORT_2);
        perror("ERR");
        exit(1);
    }

    rc = connect(sd_1,(struct sockaddr *) &servAddr1,sizeof(servAddr1));
    if(rc<0){
        perror("ERR: Cant connect");
        exit(1);
    }
    rc = connect(sd_2,(struct sockaddr *) &servAddr2,sizeof(servAddr2));
    if(rc<0){
        perror("ERR: Cant connect");
        exit(1);
    }

    char name[100];
    char birthday[9];
    printf("Input Name: ");
    scanf("%s",name);
    printf("Input Birthday (DDMMYYYY) : ");
    scanf("%s",birthday);
    strcat(name,birthday);

    rc = send(sd_1, name, strlen(name) + 1, 0);
    if(rc<0) {
      perror("cannot send data ");
      close(sd_1);
      exit(1);
    }
    printf("%s: data sent %s  at port TCP %u \n",argv[0],name,SERVER_PORT_1);

    while ( (n =read(sd_1,recvBuff_1,sizeof(recvBuff_1)-1))>0){
        recvBuff_1[n] = 0;
        if(fputs(recvBuff_1,stdout) == EOF){
            printf("\n Error : Fputs error\n");
        }
    }
    while ( (n =read(sd_2,recvBuff_2,sizeof(recvBuff_2)-1))>0){
        recvBuff_2[n] = 0;
        if(fputs(recvBuff_2,stdout) == EOF){
            printf("\n Error : Fputs error\n");
        }
        rc = send(sd_2, "Bye", strlen("Bye") + 1, 0);
        if(rc<0) {
            perror("cannot send data ");
            close(sd_2);
            exit(1);
        }
        printf("%s: data sent Bye at port TCP %u \n",argv[0],SERVER_PORT_2);
    }


    return 0;
}

