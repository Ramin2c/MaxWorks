//
//  tcpSendData.c
//  mymatrix
//
//  Created by Ramin on 2017-08-28.
//
//

#include "tcpSendData.h"
#include <stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

int sockfd,n;

struct sockaddr_in servaddr;

void setupTCPSender(int port){
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);
    
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);
    
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));
    
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
}

void sendData(char* message){
    write(sockfd, message, strlen(message)+1);
}

