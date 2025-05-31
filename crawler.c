#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

char **URLs;
int vals =0;


void sendLinks(void) {
    char *toDNS = malloc(256 * sizeof(char));
    for (int i=0; i<vals; ++i) {
        int size = (int) (strnlen(URLs[i], 256));
        for (int j=8; j<size; ++j) {
            URLs[i][j-7] = URLs[i][j];
        }
        
        char *temp = strchr(URLs[i], '/');
        int sizeReal = temp - URLs[i];
        strcpy(toDNS, URLs[i]);
        toDNS[sizeReal] = '\0';

        struct addrinfo hint;
        struct addrinfo *result;
        
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("not enough inputs written into command line");
        return 1;
    }
    if (strcmp(argv[1], "-f") == 0) {
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
        for (int i=0; i<argc-2; ++i) {
            if (i==0) {
                URLs = malloc(sizeof(char *));
            } else {
                URLs = realloc(URLs, i+1 * sizeof(char*));
            }
            URLs[vals] = malloc(256 * sizeof(char));
            strcpy(URLs[vals], argv[i+2]);
            vals++;
        }
        sendLinks();
    }
    return 0;
}
