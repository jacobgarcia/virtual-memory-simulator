//
//  FileIO.h
//  VirtualMemorySimulator
//
//  Created by Mario Garcia on 11/10/14.
//  Copyright (c) 2014 Umbrella Corporation. All rights reserved.
//

/*************************************************************************/
/*                         Global constant values                        */
/*************************************************************************/

#define NUMPARAMS 2
#define TLBSIZE 8
#define PAGETABLESIZE 128
#define FRAMESIZE 16
#define OFFSET 9


struct reference {
    unsigned int address;
    unsigned int access;
};

enum { INVALID = -1, READ, WRITE };
enum { NO = 0, YES };

struct reference GetAddress (FILE *fp);
int GetInt (FILE *fp);
void ErrorMsg (char * function, char * message);
