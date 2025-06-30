#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <errno.h>
#include <openssl/ssl.h>

char **URLs;
int vals =0;

void extractHttpsHTML(struct addrInfo *result, struct addrinfo *rp, int sfd, char *toDNS, char *route) {
    //Extract the HTML content from the HTTPS response
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        fprintf(stderr, "could not create SSL context\n");
        exit(EXIT_FAILURE);
    }
    rp = result;
    while(rp != NULL && rp->ai_addr != NULL) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            perror("error creating socket");
            rp = rp->ai_next;
            continue;
        }
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            SSL *ssl = SSL_new(ctx); 
            if (!ssl) {
                fprintf(stderr, "could not create SSL structure\n");
                exit(EXIT_FAILURE);  
            }
            if (SSL_set_fd(ssl, sfd) == 0) {
                fprintf(stderr, "could not set file descriptor for SSL\n");
                exit(EXIT_FAILURE);  
           }
            if (SSL_connect(ssl) < 1) {
                fprintf(stderr, "could not establish SSL connection\n");
                exit(EXIT_FAILURE);
            }
            char* sendInfo = malloc(1000 * sizeof(char));
            snprintf(sendInfo, 1000, 
                "GET /%s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "User-Agent: My-C-Client\r\n"
                "Connection: close\r\n"
                "\r\n",
                route, toDNS);
            
            char *buffer = malloc(3000 * sizeof(char));
            size_t sent = SSL_write(ssl, sendInfo, strlen(sendInfo));
            if (send <= 0) {
                fprinf(stderr, "could not send data\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void extractHttpHTML(struct addrinfo *result, struct addrinfo *rp, int sfd, char *toDNS, char *route) {
    // Extract the HTML content from the HTTP response
    rp = result;
    while(rp != NULL && rp->ai_addr != NULL) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            perror("error creating socket");
            rp = rp->ai_next;
            continue;
        }
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            char* sendInfo = malloc(1000 * sizeof(char));
            snprintf(sendInfo, 1000, 
                "GET /%s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "User-Agent: My-C-Client\r\n"
                "Connection: close\r\n"
                "\r\n",
                route, toDNS);
            char *buffer = malloc(3000 * sizeof(char));
            ssize_t sent = send(sfd, sendInfo, strlen(sendInfo), 0);
            if (sent == -1) {
                printf("could not send data");
                exit(EXIT_FAILURE);
            }
            ssize_t totalRecieved = 0;
            ssize_t recieved;
            struct timeval timeout = {100, 0};
            setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    
            while((recieved = recv(sfd, buffer + totalRecieved, 3000 - totalRecieved, 0)) > 0) {
                totalRecieved += recieved;
                if (totalRecieved >= 3000) {
                    printf("buffer overflow, increase buffer size, truancating data\n");
                    break;
                }
                printf("recieved %zd bytes\n", totalRecieved);
                }
            if (recieved == -1) {
                printf("could not recieve data");
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                     printf("Timeout occurred, no data received within the specified time.\n");
                 } 
                exit(EXIT_FAILURE);
            }    
            if (recieved == 0) {
                printf("server closed connection\n");
            }
            buffer[totalRecieved] = '\0'; 
            printf("Recieved data:\n%s\n", buffer);
            fflush(stdout);
            }
        else{ 
            printf("Unexpected status");
            break;
        } 
        close(sfd);
        rp = rp->ai_next; 
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
        //HTTPS links
        if (URLs[i][4] == 's') {
            for (int j=8; j<size; ++j) {
                URLs[i][j-8] = URLs[i][j];
            }
            URLs[i][size - 8] = '\0';
            char *e = strchr(URLs[i], '/');
            char *route = NULL;
            if (e != NULL && *(e + 1) != '\0') {
                route = strdup(e + 1);  
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
            s = getaddrinfo(toDNS, "https", &hint, &result);
            if (s!= 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            } 
            if (result == NULL || result->ai_addr == NULL) {
                fprintf(stderr, "Invalid address result\n");
                exit(EXIT_FAILURE);
            }
            extractHttpsHTML(result, rp, sfd, toDNS, NULL);
        //HTTP links
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
            extractHttpHTML(result, rp, sfd, toDNS, route);
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

