#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

#define ERROR 1

#define SERVER_PORT_1 12446
#define SERVER_PORT_2 22446
#define MAX_MSG 100
#define END_LINE 0x0



int main(int argc,char *argv[]){

    struct sockaddr_in cliAddr,servAddr1,servAddr2;
    char line[MAX_MSG];
    char line1[MAX_MSG];
    char sendBuff[200];
    

    int sd_1 = socket(AF_INET,SOCK_STREAM,0);
    if(sd_1==-1){
        perror("ERR: Socket 1 cant open.");
        return ERROR;
    }

    int sd_2 = socket(AF_INET,SOCK_STREAM,0);
    if(sd_2==-1){
        perror("ERR: Socket 2 cant open.");
        return ERROR;
    }

    servAddr1.sin_family = AF_INET;
    servAddr1.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr1.sin_port = htons(SERVER_PORT_1);


    if(bind(sd_1,(struct sockaddr *) &servAddr1,sizeof(servAddr1))<0){
        perror("ERR: Socket cant bind.");
        return ERROR;}
    
    servAddr2.sin_family = AF_INET;
    servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr2.sin_port = htons(SERVER_PORT_2);

    if(bind(sd_2,(struct sockaddr *) &servAddr2,sizeof(servAddr2))<0){
        perror("ERR: Socket cant bind.");
        return ERROR;}
    
    listen(sd_1,5);
    listen(sd_2,5);

    printf("%s: Open on port TCP %u\n",argv[0],SERVER_PORT_1);
    printf("%s: Open on port TCP %u\n",argv[0],SERVER_PORT_2);
    while(1){
        printf("%s: wating for data on port TCP %u\n",argv[0],SERVER_PORT_1);

        int clilen = sizeof(cliAddr);
        int newSd =accept(sd_1,(struct sockaddr *) &cliAddr,&clilen);
        int newSd_2 =accept(sd_2,(struct sockaddr *) &cliAddr,&clilen);
        if(newSd<0){
            perror("ERR: Socket cant accept connection.");
            return ERROR;
        }
        if(newSd_2<0){
            perror("ERR: Socket cant accept connection.");
            return ERROR;
        }
        memset(line,0x0,MAX_MSG);
        while (read_line(newSd,line)!=ERROR)
        {
            printf("%s: received from %s:TCP %d : %s\n",argv[0],
            inet_ntoa(cliAddr.sin_addr),
            ntohs(cliAddr.sin_port),
            line);  

            snprintf(sendBuff,sizeof(sendBuff),"Server received \n");
            if(send(newSd,sendBuff,strlen(sendBuff),0)== -1){
              printf("ERR:failed to send");
            }
            close(newSd);

            int num_index =-1;
            for(int i =0;i<strlen(line);i++){
              if(isdigit(line[i])){
                num_index = i;
                break;
              }
            }
            char name[100],date[9];
            strncpy(name,line,num_index);
            strcpy(date,line+num_index);

            printf("Data Name is: %s \n",name);
            printf("And Birthday is: %s \n",date);

            char year[5];
            strcpy(year,date+4);
            printf("Data Year is: %s \n",year);

            int year_int = atoi(year);
            year_int = year_int - 543;
            snprintf(year,10,"%d\n",year_int);
            printf("Data Year After Convert is: %s \n",year);
            if(send(newSd_2,year,strlen(year),0)== -1){
              printf("ERR:failed to send");
            }
            if(recv(newSd_2,line1,1024,0)==-1){
              printf("ERR:failed to receive");
            }

            printf("%s\n",line1);
            
            memset(line,0x0,MAX_MSG);
            memset(line1,0x0,MAX_MSG);
            close(newSd_2);




        }
                

    }
}




int read_line(int newSd, char *line_to_return) {
  
  static int rcv_ptr=0;
  static char rcv_msg[MAX_MSG];
  static int n;
  int offset;

  offset=0;

  while(1) {
    if(rcv_ptr==0) {
      /* read data from socket */
      memset(rcv_msg,0x0,MAX_MSG); /* init buffer */
      n = recv(newSd, rcv_msg, MAX_MSG, 0); /* wait for data */
      if (n<0) {
	// perror(" cannot receive data ");
	return ERROR;
      } else if (n==0) {
	printf(" connection closed by client\n");
	close(newSd);
	return ERROR;
      }
    }
  
    /* if new data read on socket */
    /* OR */
    /* if another line is still in buffer */

    /* copy line into 'line_to_return' */
    while(*(rcv_msg+rcv_ptr)!=END_LINE && rcv_ptr<n) {
      memcpy(line_to_return+offset,rcv_msg+rcv_ptr,1);
      offset++;
      rcv_ptr++;
    }
    
    /* end of line + end of buffer => return line */
    if(rcv_ptr==n-1) { 
      /* set last byte to END_LINE */
      *(line_to_return+offset)=END_LINE;
      rcv_ptr=0;
      return ++offset;
    } 
    
    /* end of line but still some data in buffer => return line */
    if(rcv_ptr <n-1) {
      /* set last byte to END_LINE */
      *(line_to_return+offset)=END_LINE;
      rcv_ptr++;
      return ++offset;
    }

    /* end of buffer but line is not ended => */
    /*  wait for more data to arrive on socket */
    if(rcv_ptr == n) {
      rcv_ptr = 0;
    } 
    
  } /* while */
}






