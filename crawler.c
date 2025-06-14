#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

char **URLs;
int vals =0;
//NEW COMMIT

void extractHTML(struct addrinfo *result, struct addrinfo *rp, int sfd) {
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        //check this logic, should continue until succesful connection, 
        //not quit at first failure, maybe add while loop or quit out of 
        //for loop with valid logic once connection is made
        if (sfd == -1) {
            printf("error creating socket connection\n");
        }   
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            if (rp == NULL) {
                fprintf(stderr, "could not connect\n");
                exit(EXIT_FAILURE);
            }
            //IMPLEMENT THE WRITE METHOD BEFORE MOVING TO RECIEVED
            char request[4000];


            char *buffer = malloc(3000 * sizeof(char));
            ssize_t recieved = recv(sfd, buffer, 3000, 0);
            if (recieved == -1) {
                printf("could not recieve data");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("error connecting to server");
        }
        close(sfd);
    }
}

void sendLinks(void) {
    for (int i=0; i<vals; ++i) {

        char *toDNS = malloc(256 * sizeof(char));
        
        struct addrinfo hint;
        struct addrinfo *result;
        struct addrinfo *rp;
        
        memset(&hint, 0, sizeof(hint));
        
        int s;
        int sfd;
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_flags = 0;
        hint.ai_protocol = 0;
        
        int size = (int) (strnlen(URLs[i], 256));
        if (URLs[i][4] == 's') {
            printf("came here 2");

            for (int j=8; j<size; ++j) {
                URLs[i][j-8] = URLs[i][j];
            }
            
            char *temp = strchr(URLs[i], '/');
            int sizeReal = temp - URLs[i];
            strcpy(toDNS, URLs[i]);
            toDNS[sizeReal] = '\0';
            s = getaddrinfo(toDNS, "https", &hint, &result);
            
            if (s!= 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            } 
            extractHTML(result, rp, sfd);
        } else {
            for (int j=7; j<size; ++j) {
                URLs[i][j-7] = URLs[i][j];
            }
            char *e;
            if (strchr(URLs[i], '/') != NULL) {
                e = strchr(URLs[i], '/'); 
                if (e != NULL) {
                    printf("came here");
                    fflush(stdout);
                    int offset = e - URLs[i];
                    char *route = malloc(offset + 1);
                    strncpy(route, URLs[i], offset);
                    route[offset] = '\0';
                }
            }
            URLs[i][size-7] = '\0';
            char *temp = strchr(URLs[i], '/');
            int sizeReal = temp - URLs[i];
            strcpy(toDNS, URLs[i]);
            toDNS[sizeReal] = '\0';
            s = getaddrinfo(toDNS, "http", &hint, &result);
            
            if (s!= 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            } 
            extractHTML(result, rp, sfd);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("not enough inputs written into command line");
        return 1;
    }
    if (strcmp(argv[1], "-f") == 0) {
        //TEST ONE
        printf("child came here 1");
        FILE *fp = fopen(argv[2], "r");
        char *tempCpy = malloc(256 * sizeof(char));
        while(fgets(tempCpy, 256, fp) != NULL) {
            if (vals == 0) {
                URLs = malloc(sizeof(char *));
            } else {
                URLs = realloc(URLs, vals * sizeof(char *));
            }
            URLs[vals] = malloc(256 * sizeof(char));
            strcpy(URLs[vals], tempCpy);
            vals++;
        }
        sendLinks();
    }

    if (strcmp(argv[1], "-c") == 0) {
        for (int i=2; i<argc-1; ++i) {
            if (i==2) {
                URLs = malloc(sizeof(char *));
            } else {
                URLs = realloc(URLs, 256 * sizeof(char*));
            }
            URLs[vals] = malloc(256 * sizeof(char));
            strcpy(URLs[vals], argv[i]);
            vals++;
        }
       // printf("%s", URLs[0]);
        sendLinks();
    }
    return 0;
}

