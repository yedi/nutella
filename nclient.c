/* 

nclient.c
Nutella Front End

-Onyedi Anyansi

*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "msock.h"

#define CHECK_PORT 16007
#define CHECK_ADDR "239.0.0.1"
#define FOUND_PORT 16008
#define FOUND_ADDR "239.0.0.2"
#define MESSAGE_LEN 75
#define SLEEP_TIME 3
#define MOVIE_TITLE_LEN 25
#define FOUND_MSG_LEN 100
//#define HOST_NAME_MAX 75

int main(int argc) {
    char message[MESSAGE_LEN];
    char movie_title[MOVIE_TITLE_LEN];
    int len, check_sock, found_sock, cnt;

    int stream_port;
    char *stream_addr;

    int stream_sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buffer[1440];
    char *stream_hostname;

    printf("Nutella Client Started\n");
    while(1) {
        printf("Enter Movie Name: ");
        scanf("%s", movie_title);
        printf("Sending search request for the movie %s\n", movie_title);

        /* set up socket */
        if ((check_sock=msockcreate(SEND, CHECK_ADDR, CHECK_PORT)) < 0) {
            perror("msockcreate");
            exit(1);
        }

        /* multicast message */
        cnt = msend(check_sock, movie_title, strlen(movie_title)+1);
        if (cnt < 0) {
            perror("msend");
            exit(1);
        }

        printf("Waiting for response\n");

        /* set up socket */
        if ((found_sock=msockcreate(RECV, FOUND_ADDR, FOUND_PORT)) < 0) {
            perror("msockcreate");
            exit(1);
        }

        char found_message[FOUND_MSG_LEN];

        /* get multicasted message */
        cnt = mrecv(found_sock, found_message, FOUND_MSG_LEN);
        if (cnt < 0) {
            perror("mrecv");
            exit(1);
        } 
        else if (cnt==0) {
            return 0;
        }

        stream_port = atoi(strtok(found_message," "));
        stream_addr = strtok (NULL, " ");
        stream_hostname = strtok (NULL, " ");

        printf("Received port(%d), IP(%s) and hostname(%s) from server\n", stream_port, stream_addr, stream_hostname);

        stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (stream_sock < 0) printf("socket error");

        server.sin_family = AF_INET;

        /*
        bcopy((char *)stream_addr, 
            (char *)&server.sin_addr,
             strlen(stream_addr));
        */
        //hp = gethostbyaddr(stream_addr);
        /*
        if (inet_aton(stream_addr, &server.sin_addr) == 0) {
            printf("inet_aton\n");
            exit(EXIT_FAILURE);
        }

    /*
        hp = gethostbyaddr( (char *)&server.sin_addr.s_addr,
                        sizeof(server.sin_addr.s_addr),AF_INET);
        if (hp==0) error("Unknown host");
        */

        hp = gethostbyname(stream_hostname);
        bcopy((char *)hp->h_addr, 
            (char *)&server.sin_addr,
             hp->h_length);

        server.sin_port = htons(stream_port);
        length=sizeof(struct sockaddr_in);

        bzero(buffer,1440);
        sprintf(buffer, "stream_request:%s", movie_title);

        n=sendto(stream_sock,buffer,
                strlen(buffer),0,(const struct sockaddr *)&server,length);
        if (n < 0) printf("Sendto error");

        while (1) {
            n = recvfrom(stream_sock,buffer,1440,0,(struct sockaddr *)&from, &length);
            if (n < 0) {
                printf("recvfrom error");
                break;
            }
            if (strcmp(buffer, "End Movie") == 0) {
                printf("\033[2J");
                printf("\033[0;0f");
                break;
            }
            printf("\033[2J");
            printf("\033[0;0f");
            printf("%s", buffer);
            memset(&buffer[0], 0, sizeof(buffer));
            //write(1,"Got an ack: ",12);
            //write(1,buffer,n);
        }
    msockdestroy(check_sock);
    msockdestroy(found_sock);
    close(stream_sock);
    }
}
