#include "hw2.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 

int main(int argc, char **argv) {
    int o;
    int flag1 = 0;
    int flag2 = 0;
    int flag3 = 0;
    int flag4 = 0;
    int flag5 = 0;
    while((o = getopt(argc, argv, "i:o:c:p:r")) != -1){
        switch(o){
            case 'i':
                flag1 = 1;
                break;
            case 'o':
                flag2 = 1;
                break;
            case 'c':
                flag3 = 1;
                break;
            case 'p':
                flag4 = 1;
                break;
            case 'r':
                flag5 = 1;
                break;
            default:
            return UNRECOGNIZED_ARGUMENT;
        }
    if(!flag1 || !flag2 || !flag3 || !flag4 || !flag5){
        return MISSING_ARGUMENT;
    }
    }

    bool flagi = false; 
    bool flago = false;
    bool flagc = false;
    bool flagr = false;
    bool flagp = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            if (flagi) {
                return DUPLICATE_ARGUMENT; 
            }
            flagi = true;
        } 
        else if (strcmp(argv[i], "-o") == 0) {
            if (flago) {
                return DUPLICATE_ARGUMENT; 
            }
            flago = true;
        }
         else if (strcmp(argv[i], "-c") == 0) {
            if (flagc) {
                return DUPLICATE_ARGUMENT; 
            }
            flagc = true;
        } 
        else if (strcmp(argv[i], "-r") == 0) {
            if (flagr) {
                return DUPLICATE_ARGUMENT; 
            }
            flagr = true;
        } 
        else if (strcmp(argv[i], "-p") == 0) {
            if (flagp) {
                return DUPLICATE_ARGUMENT; 
            }
            flagp = true;
        }
    
    return 0;
}
    
    (void)argc;
    (void)argv;

    return 0;
}
