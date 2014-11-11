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
unsigned int events = 0;
unsigned int tableEntry;
unsigned int frameEntry;
unsigned int hits = 0;
unsigned int found = NO;
unsigned int tlbFull = YES;
unsigned long int accessTime = 0;
unsigned int lru = 0;
unsigned int hitCompare;
unsigned int tlbTime = 0;
unsigned int pageFaultPenalty = 0;
unsigned int memoryTime = 0;
unsigned int pageIn = 0;
unsigned int pageOut = 0;

struct TLBStructure TLB[TLBSIZE];
struct TLBStructure virtualTLB[TLBSIZE];
struct memoryStructure memory[FRAMESIZE];
struct memoryStructure virtualMemory[FRAMESIZE];
/*************************************************************************/
/*                            Main entry point                           */
/*************************************************************************/

void CopyTLB(){
    /* Copy all elements in the list*/
    for (int i = 0; i < TLBSIZE; i++)
        virtualTLB[i] = TLB[i];
}

void CopyMemory(){
    /* Copy all elements in the list*/
    for (int i = 0; i < FRAMESIZE; i++)
        virtualMemory[i] = memory[i];
}

void Evict(){
    CopyTLB();
    for (int i = 0; i < TLBSIZE; i++){
        for (int j = i + 1; j < TLBSIZE; j++){
            /* Exchange the processes if the arrival time of the first process is higher than the second process */
            if (virtualTLB[i].lru > virtualTLB[j].lru){
                struct TLBStructure temporal = virtualTLB[i]; /* Create a temporal structure that stores a process
                                                        in order to not lose it during the exchange */
                virtualTLB[i] =  virtualTLB[j];
                virtualTLB[j] = temporal;
            }
        }
    }
    
    for (int tlbEntry = 0; tlbEntry < TLBSIZE; tlbEntry++) {
        if (virtualTLB[0].pageNumber == TLB[tlbEntry].pageNumber) {
            tableEntry = tlbEntry;
            tlbEntry = TLBSIZE;
        }
    }
}

void PageOut(int address, int access){
    
    if (memory[frameEntry].state == 'D'){
        pageIn++;
        pageOut++;
    }
    else
        pageIn++;
    
    TLB[tableEntry].pageNumber = address;
    memory[frameEntry].pageNumber = address;
    
    TLB[tableEntry].frameNumber = memory[frameEntry].frameNumber;
    TLB[tableEntry].lru = events;
    memory[frameEntry].lru = events;
    
    if(access == READ){
        memory[frameEntry].access = 'R';
        TLB[tableEntry].access = 'R';
        memory[frameEntry].state = 'C';
    }
    else{
        memory[frameEntry].access = 'W';
        TLB[tableEntry].access = 'W';
        memory[frameEntry].state = 'D';
    }
    
    
}

void EvictFrames(int address, int access){
    CopyMemory();
    for (int i = 0; i < FRAMESIZE; i++){
        for (int j = i + 1; j < FRAMESIZE; j++){
            /* Exchange the processes if the arrival time of the first process is higher than the second process */
            if (virtualMemory[i].lru > virtualMemory[j].lru){
                struct memoryStructure temporal = virtualMemory[i]; /* Create a temporal structure that stores a process
                                                               in order to not lose it during the exchange */
                virtualMemory[i] =  virtualMemory[j];
                virtualMemory[j] = temporal;
            }
        }
    }
    
    for (int memoryEntry = 0; memoryEntry < FRAMESIZE; memoryEntry++) {
        if (virtualMemory[0].pageNumber == memory[memoryEntry].pageNumber) {
            frameEntry = memoryEntry;
            memoryEntry = FRAMESIZE;
        }
    }
    
    PageOut(address, access);
}

void PageIn(int address, int access){
    unsigned int fullFrames = 1;
    accessTime = accessTime + memoryTime;
    TLB[tableEntry].state = 'V';
    
    for (int i = 0; i < FRAMESIZE; i++) {
        if (memory[i].pageNumber == INVALID || memory[i].pageNumber == address){
            
            if (memory[i].pageNumber != address)
                pageIn++;
            
            TLB[tableEntry].pageNumber = address;
            memory[i].pageNumber = address;
            
            TLB[tableEntry].frameNumber = memory[i].frameNumber;
            TLB[tableEntry].lru = events;
            memory[i].lru = events;
            
            if(access == READ){
                memory[i].access = 'R';
                TLB[tableEntry].access = 'R';
                memory[i].state = 'C';
            }
            else{
                memory[i].access = 'W';
                TLB[tableEntry].access = 'W';
                memory[i].state = 'D';
            }
            frameEntry = i;
            fullFrames = 0;
            i = FRAMESIZE; /* Break condition*/
        }
    }
    if (fullFrames == 1) {
        EvictFrames(address, access);
    }

}

void InitializeMemory(){
    for (int i = 0; i < FRAMESIZE; i++){
        memory[i].pageNumber = INVALID;
        memory[i].frameNumber = i;
    }
}

void GetTimeParameters(){
    memoryTime = GetInt(fp);
    
    tlbTime = GetInt(fp);
    
    pageFaultPenalty = GetInt(fp);
}

void CopyToFrame(){
    for (int frameEntry = 0; frameEntry < FRAMESIZE; frameEntry++) {
        for (int tableEntry = 0; tableEntry < TLBSIZE; tableEntry++) {
            if(memory[frameEntry].pageNumber == TLB[tableEntry].pageNumber){
                memory[frameEntry].lru = TLB[tableEntry].lru;
                memory[frameEntry].access = TLB[tableEntry].access;
            }
        }
    }
}

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
            
            /* Initialize Memory */
            InitializeMemory();
            
            GetTimeParameters();
            
            while(!feof(fp)){
                struct reference theReference = GetAddress(fp);
                events++;
                
                hitCompare = hits;
                /* Search for the address in the TLB */
                accessTime = accessTime + tlbTime;
                for (int i = 0; i < TLBSIZE; i++) {
                    if(theReference.address == TLB[i].pageNumber){
                        tableEntry = i;
                        i = TLBSIZE; /* Break condition */
                        found = YES;
                        tlbFull = NO;
                        hits++;
                        TLB[tableEntry].lru = events;
                        CopyToFrame();
                        if(theReference.access == READ)
                            TLB[tableEntry].access = 'R';
                        else{
                            TLB[tableEntry].access = 'W';
                            memory[tableEntry].state = 'D';
                        }
                    }
                    else if (TLB[i].pageNumber == INVALID){
                        tableEntry = i;
                        tlbFull = NO;
                        i = TLBSIZE; /* Break condition */

                        PageIn(theReference.address, theReference.access);
                    }
                }
                
                if (tlbFull == YES) {
                    Evict();
                    PageIn(theReference.address, theReference.access);
                } else if (found == NO) {
                    /* Search for the address in the Page Table */
               
                }
                
                tlbFull = YES;
            
                #ifdef DEBUG
                if(hitCompare == hits){
                        printf("\nTLB Contents\n\n");
                        for (int tableEntry = 0; tableEntry < TLBSIZE; tableEntry++) {
                            if (TLB[tableEntry].pageNumber == INVALID)
                                printf("Page #: -   Frame #: -   LRU: -   Access: -   State: -\n");
                            else
                                printf("Page #: %3d   Frame #: %2d   LRU: %2d   Access: %c   State: %c\n", TLB[tableEntry].pageNumber, TLB[tableEntry].frameNumber, TLB[tableEntry].lru, TLB[tableEntry].access, TLB[tableEntry].state);
                        }
            
                        printf("\n\nFrame Contents\n\n");
                        for (int frameEntry = 0; frameEntry < FRAMESIZE; frameEntry++) {
                            if (memory[frameEntry].pageNumber == INVALID)
                                printf("Frame #:  -   Page #:   -   LRU:  -    Access: -   State: -\n");
                            else
                                printf("Frame #: %2d   Page #: %3d   LRU: %3d   Access: %c   State: %c\n", memory[frameEntry].frameNumber, memory[frameEntry].pageNumber, memory[frameEntry].lru, memory[frameEntry].access, memory[frameEntry].state);
                        }
                }
                #endif
            }
            accessTime = accessTime + pageFaultPenalty * pageIn;
            printf("\nTotal number of events: %d\n", events);
            printf("Average access time: %f\n", accessTime/(double)events);
            printf("Page Ins: %d\n", pageIn);
            printf("Page Outs: %d\n", pageOut);
            printf("TLB Hit Ratio: %f\n", hits/(double)events);
        }
    }
}

