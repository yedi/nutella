/* 

nserver.cpp
Nutella Passive Server

-Onyedi Anyansi

*/

#include <stdio.h>
#include <time.h>
#include <string.h>
extern "C" {
#include "msock.h"
}

#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <string.h>

#define CHECK_PORT 16007
#define CHECK_ADDR "239.0.0.1"
#define FOUND_PORT 16008
#define FOUND_ADDR "239.0.0.2"
#define STREAM_PORT 16009
#define STREAM_ADDR "239.0.0.3"
#define MESSAGE_LEN 75
#define SLEEP_TIME 3
#define MOVIE_TITLE_LEN 25
//#define HOST_NAME_MAX 75

char *lowercase(char *string) {
    char *p;
    for (p = string; *p != '\0'; ++p)
    {
        *p = tolower(*p);
    }
    return string;
}

int main(int argc) {
    char message[MESSAGE_LEN];
    char movie_title[MOVIE_TITLE_LEN];
    int len, check_sock, found_sock, cnt;

    int sock, length, n;
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024];
    struct hostent *hp;

    printf("Nutella Server Started\n");

    printf("Listening for movie requests\n");

    char hostname[HOST_NAME_MAX];

    if (gethostname(hostname, sizeof hostname) != 0)
    {
        printf("hostname error");
        exit(1);
    }

    while (1) {

        /* set up socket */
        if ((check_sock=msockcreate(RECV, CHECK_ADDR, CHECK_PORT)) < 0) {
            perror("msockcreate");
            exit(1);
        }

        /* get multicasted message */
        cnt = mrecv(check_sock, movie_title, MOVIE_TITLE_LEN);
        if (cnt < 0) {
            perror("mrecv");
            exit(1);
        } 
        else if (cnt==0) {
            return 0;
        }
        printf("Received search request for \"%s\"\n", movie_title);

        char *nut_file;
        int num_movies;
        char movies[100][25];
        char movie_locs[100][25];
        char frame[1440];
        char temp_frame[960];

        fstream inStream;
        nut_file = ".nutella";

        inStream.open(nut_file, ios :: in);
        if(inStream.fail())
        {
            //return false;
            cout << "couldn't open\n";
            return 0;
        }

        //get number of movies from .nutella file
        inStream >> num_movies;

        int i;
        for (i = 0; i < num_movies; i++) {
            inStream >> movies[i];
            inStream >> movie_locs[i];
        }

        inStream.close();

        int movie_loc;
        int movie_found = 0;

        for (i = 0; i < num_movies; i++) {
            if (strcmp(movies[i], lowercase(movie_title)) == 0) {
                //printf("Found the movie %s\n", movie_title);
                movie_found = 1;
                movie_loc = i;
                break;
            }
        }

        if (movie_found != 1) {
            printf("Couldn't find the movie %s\n", movie_title);
            return 0;
        }
        else {
            printf("Found the movie %s\n", movie_title);

            /* set up socket */
            if ((found_sock=msockcreate(SEND, FOUND_ADDR, FOUND_PORT)) < 0) {
                perror("msockcreate");
                exit(1);
            }

            printf("Sending port %d and address %s to client \n", STREAM_PORT, STREAM_ADDR);
            
            char found_message[100];
            
            sprintf(found_message, "%d %s %s", STREAM_PORT, STREAM_ADDR, hostname);
            
            /* multicast message */
            cnt = msend(found_sock, found_message, strlen(found_message)+1);
            if (cnt < 0) {
                perror("msend");
                exit(1);
            }
        }

        sock=socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) printf("Opening socket error");
        length = sizeof(server);
        bzero(&server,length);
        server.sin_family=AF_INET;

        //server.sin_addr.s_addr=STREAM_ADDR;
        if (inet_aton(STREAM_ADDR, &server.sin_addr) == 0) {
            perror("inet_aton");
            exit(EXIT_FAILURE);
        }

        /*
        bcopy((char *)STREAM_ADDR, 
            (char *)&server.sin_addr,
            strlen(STREAM_ADDR));
            */
        //hp = gethostbyaddr(stream_addr);


        hp = gethostbyname(hostname);
        if (hp==0) printf("Unknown host");

        bcopy((char *)hp->h_addr, 
            (char *)&server.sin_addr,
             hp->h_length);
        server.sin_port=htons(STREAM_PORT);

        if (bind(sock,(struct sockaddr *)&server,length)<0) 
            printf("binding error");
        fromlen = sizeof(struct sockaddr_in);

        n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
        if (n < 0) printf("recvfrom");
        write(1,"Received a datagram: ",21);
        write(1,buf,n);


        char *filename = movie_locs[movie_loc];
        FILE *file = fopen ( filename, "r" );
        if ( file != NULL )
        {
            char line [ 512 ]; /* or other suitable maximum line size */
            strcpy(frame, "");
            strcpy(temp_frame, "");
            bzero(frame ,1440);
            
            struct timespec tim, tim2;
            tim.tv_sec = 0;
            tim.tv_nsec = 100000000;
       
            while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
            {
                //printf("%s", frame);
                if (strcmp(line, "end\n") == 0) {

                    //bzero(buf ,256);
                    //sprintf(buffer, "stream_request:%s", movie_title);
                    printf("Sending: \n%s\n", frame);
                    n = sendto(sock, frame, strlen(frame),
                          0,(struct sockaddr *)&from,fromlen);
                    //sleep(.09);
                    nanosleep(&tim , &tim2);
                    if (n  < 0) printf("sendto error");
                    memset(&frame[0], 0, sizeof(frame));
                    printf("Starting new cycle \n");
                }
                else strcat(frame, line);
                //strcat(frame, "\n");
                //fputs ( line, stdout ); /* write the line */
            }

            fclose ( file );
            n = sendto(sock, "End Movie", 9,
                          0,(struct sockaddr *)&from,fromlen);
            }
        else
        {
            printf ( "file didn't open" ); /* why didn't the file open? */
        }
    /*
        while (1) {
            n = sendto(sock,"Got your message\n",17,
                      0,(struct sockaddr *)&from,fromlen);
            if (n  < 0) printf("sendto error");

            return 0;
        }
    */
    msockdestroy(check_sock);
    msockdestroy(found_sock);
    close(sock);
    }
}
