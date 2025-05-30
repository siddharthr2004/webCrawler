#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **URLs;
int vals;

void getPages(void) {

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
        getPages();
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
        getPages();
    }
    return 0;
}
