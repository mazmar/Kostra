/* 
 * File:   Transfer.h
 * Author: mazmart
 *
 * Created on November 17, 2012, 12:11 PM
 */

#ifndef TRANSFER_H
#define	TRANSFER_H

#include "Kostra.h"
#include "LinkedStack.h"

extern Uzel * uzly;

class Transfer {
public:
    int size;
    int stackSize;
    int * transfer;

    Transfer(LinkedStack<Kostra> * split);
    Transfer(int * transfer, int * size);

    LinkedStack<Kostra> * unpack();

    void print();
};

#endif	/* TRANSFER_H */

