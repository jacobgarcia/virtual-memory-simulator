//
//  Structures.h
//  VirtualMemorySimulator
//
//  Created by Mario Garcia on 11/10/14.
//  Copyright (c) 2014 Umbrella Corporation. All rights reserved.
//

struct TLBStructure {
    unsigned int pageNumber, frameNumber, lru;
    char access, state;
};