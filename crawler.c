#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

char **URLs;
int vals =0;

void extractHTML(struct addrinfo *result, struct addrinfo *rp, int sfd) {
    rp = result;
    while(rp != NULL && rp->ai_addr != NULL) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            perror("error creating socket");
            rp = rp->ai_next;
            continue;
        }
        int status = connect(sfd, rp->ai_addr, rp->ai_addrlen);
        printf("status: %d\n", status);
        if (status == 0) {
            break;
        }
        rp = rp->ai_next; 
    }
    char *buffer = malloc(3000 * sizeof(char));
    printf("made it here\n");
    printf("sfd: %d\n", sfd);
    fflush(stdout);
    ssize_t recieved = recv(sfd, buffer, 3000, 0);
    //CHECK HERE
    printf("%d\n", (int)recieved);
    fflush(stdout);
    if (recieved == -1) {
        printf("could not recieve data");
        exit(EXIT_FAILURE);
    }
    close(sfd);
    
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
        //for https links
        if (URLs[i][4] == 's') {

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
        //solely for http links
        } else {
            for (int j=7; j<size; ++j) {
                URLs[i][j-7] = URLs[i][j];
            }
            URLs[i][size - 7] = '\0';
            char *e = strchr(URLs[i], '/');
            char *route = NULL;

            if (e != NULL && *(e + 1) != '\0') {
                route = strdup(e + 1);  // duplicate everything after the first '/'
                if (route == NULL) {
                    perror("strdup");
                    exit(EXIT_FAILURE);
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
            if (result == NULL || result->ai_addr == NULL) {
                fprintf(stderr, "Invalid address result\n");
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
        sendLinks();
    }
    return 0;
}

