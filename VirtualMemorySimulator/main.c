//
//  main.c
//  VirtualMemorySimulator
//
//  Created by Mario Garcia on 11/10/14.
//  Copyright (c) 2014 Umbrella Corporation. All rights reserved.
//

#include <stdlib.h>              /* Used for getc() and feof() functions */
#include <stdio.h>
#include <ctype.h>                    /* Used for the isdigit() function */
#include "FileIO.h"
#include "Structures.h"


FILE * fp;
unsigned long int events;

struct TLBStructure TLB[TLBSIZE];

/*************************************************************************/
/*                            Main entry point                           */
/*************************************************************************/

int main(int argc, const char * argv[])
{
    /* Check if the parameters in the main function are not empty */
    if (argc < NUMPARAMS){
        printf("Need a file with the process information\n\n");
        printf("Abnormal termination\n");
        return (EXIT_FAILURE);
    }
    else {
        
        /* Open the file and check that it exists */
        fp = fopen (argv[1],"r");       /* Open file for read operation */
        if (!fp)                            /* The file does not exists */
            ErrorMsg("'main'","Filename does not exist or is corrupted\n");
        
        else {
            
            /* Initialize TLB */
            for (int i = 0; i < TLBSIZE; i++)
                TLB[i].pageNumber = INVALID;

            struct reference theReference = GetInt(fp);
            #ifdef DEBUG
                printf("TLB Contents\n\n");
                for (int tableEntry = 0; tableEntry < TLBSIZE; tableEntry++) {
                    if (TLB[tableEntry].pageNumber == INVALID)
                        printf("Page #: |   Frame #: |   LRU: |   Access: |   State: |\n");
                    else
                        printf("Page #: %d   Frame #: %d   LRU: %d   Access: %c   State: %c\n", TLB[tableEntry].pageNumber, TLB[tableEntry].frameNumber, TLB[tableEntry].lru, TLB[tableEntry].access, TLB[tableEntry].state);
                }
            #endif
            
            /* Search for the address in the TLB */
            
            
            //struct reference theReference = GetInt(fp);
            printf("%d %d\n", theReference.address, theReference.access);
            events++;
            
        }
    }
}

