/* 
 * File:   Transfer.cpp
 * Author: mazmart
 * 
 * Created on November 17, 2012, 12:11 PM
 */
#include <iostream>
#include "Transfer.h"
#include "LinkedStack.h"
#include "Kostra.h"

const int GRAF_SIZE = 15;

Transfer::Transfer(LinkedStack<Kostra> * split) {
    this->size = (GRAF_SIZE * 3) * split->size + split->size;
    this->transfer = new int[this->size];
    for (int i = 0; i < this->size; i++) {
        transfer[i] = -1;
    }


    cout << "--------Transfering size: -------" << this->size;

    Node<Kostra> * n = split->front;

    int i = 0;
    while (n != NULL) {
        this->transfer[(i * (3 * GRAF_SIZE + 1))] = n->k->krok;
        int j = 1;
        for (vector<Uzel>::iterator it = n->k->kostra.begin(); it != n->k->kostra.end(); it++) {
            this->transfer[(i * (3 * GRAF_SIZE + 1)) + j] = it->name;
            this->transfer[ GRAF_SIZE + (i * (3 * GRAF_SIZE + 1)) + j] = it->current;
            this->transfer[ 2 * GRAF_SIZE + (i * (3 * GRAF_SIZE + 1)) + j] = it->pridane ? it->pridane : 0;
            j++;
        }
        i++;
        n = n->next;

    }

}

Transfer::Transfer(int * transfer, int * size) {
    this->transfer = transfer;
    this->size = *size;
}

Transfer::Transfer(int size) {
    this->size = size;
    this->transfer = new int[size];
}

LinkedStack<Kostra> * Transfer::unpack() {
    LinkedStack<Kostra> * s = new LinkedStack<Kostra > ();
    for (int i = 0; i < (this->size / (3 * GRAF_SIZE + 1)); i++) {
        int currLevel = 0;
        Kostra * k = new Kostra(this->transfer[(i * (3 * GRAF_SIZE + 1))]);
        for (int j = 0; j < GRAF_SIZE; j++) {
            int uzel = this->transfer[(i * (3 * GRAF_SIZE + 1)) + 1 + j];
            if (uzel == -1) break;
            Uzel u = uzly[uzel];
            u.current = this->transfer[(i * (3 * GRAF_SIZE + 1)) + 1 + GRAF_SIZE + j];
            u.pridane = this->transfer[1 + 2 * GRAF_SIZE + (i * (3 * GRAF_SIZE + 1)) + j];
            k->push(&u);
        }
        
        s->add(k);
    }
    return s;
}

void Transfer::print() {
    cout << "\n transfer\n";
    int t;
    for (int i = 0; i < this->size; i++) {
        if (i % 25 == 0) {
            cout << "\nKostra krokov: " << transfer[i] << "\n";
            t = 0;
            continue;
        }
        if (t++ % 8 == 0) {
            cout << "\n";
        }
        cout << this->transfer[i] << " ";


    }

}

