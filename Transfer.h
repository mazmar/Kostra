/* 
 * File:   Transfer.h
 * Author: mazmart
 *
 * Created on November 17, 2012, 12:11 PM
 */


#include "Kostra.h"
#include "LinkedStack.h"

#ifndef TRANSFER_H
#define	TRANSFER_H

extern Uzel * uzly;

class Transfer {
public:
    int size;
    int stackSize;
    int * transfer;

    Transfer(LinkedStack<Kostra> * split);
    Transfer(int * transfer, int * size);
    Transfer(int);

    LinkedStack<Kostra> * unpack();

    void print();
};

#endif	/* TRANSFER_H */

