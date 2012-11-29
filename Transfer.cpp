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

Transfer::Transfer(LinkedStack<Kostra> * split) {
    this->size = (VERTICES + 1) * split->size;
    this->transfer = new short[this->size];
    for (int i = 0; i < this->size; i++) {
        transfer[i] = -1;
    }


    file << "--------Transfering size: -------" << this->size << "\n";

    Node<Kostra> * n = split->front;

    int i = 0;
    while (n != NULL) {
        this->transfer[(i * (VERTICES + 1))] = n->k->krok;
        int j = 1;
        for (vector<Uzel>::iterator it = n->k->kostra.begin(); it != n->k->kostra.end(); it++) {
            this->transfer[(i * (VERTICES + 1)) + (it->name + 1)] = it->pridane;
        }
        i++;
        n = n->next;

    }

}

Transfer::Transfer() {
}

Transfer::Transfer(short * transfer, int * size) {
    this->transfer = transfer;
    this->size = *size;
}

Transfer::Transfer(int size) {
    this->size = size;
    this->transfer = new short[size];
}

Transfer::~Transfer() {
    delete[] this->transfer;
}

LinkedStack<Kostra> * Transfer::unpack() {
    LinkedStack<Kostra> * s = new LinkedStack<Kostra > ();
    //    for (int i = 0; i < (this->size / (3 * VERTICES + 1)); i++) {
    //        int currLevel = 0;
    //        Kostra * k = new Kostra(this->transfer[(i * (3 * VERTICES + 1))]);
    //        for (int j = 0; j < VERTICES; j++) {
    //            int uzel = this->transfer[(i * (3 * VERTICES + 1)) + 1 + j];
    //            if (uzel == -1) break;
    //            Uzel u = uzly[uzel];
    //            u.current = this->transfer[(i * (3 * VERTICES + 1)) + 1 + VERTICES + j];
    //            u.pridane = this->transfer[1 + 2 * VERTICES + (i * (3 * VERTICES + 1)) + j];
    //            k->push(&u);
    //        }
    //
    //        s->add(k);
    //    }

    for (int i = 0; i < (this->size / (VERTICES + 1)); i++) {

        Kostra * kostra = new Kostra(this->transfer[(i * (VERTICES + 1))]);

        int max = (VERTICES / 2) * VERTICES;
        for (int j = 0; j < max; j++) {
            for (int k = 1; k < VERTICES + 1; k++) {
                if (this->transfer[(i * (VERTICES + 1)) + k] == j) {
                    Uzel u = uzly[k - 1];
                    u.current = 0;
                    u.pridane = this->transfer[(i * (VERTICES + 1)) + k];
                    kostra->push(&u);
                    break;
                }
            }
        }

        s->add(kostra);


    }
    file << "\n Prijaty Stack\n";
    s->print();
    return s;
}

void Transfer::print() {
    file << "\n transfer\n";
    int t;
    for (int i = 0; i < this->size; i++) {
        if (i % (VERTICES + 1) == 0) {
            file << "\nKostra krokov: " << transfer[i] << "\n";
            t = 0;
            continue;
        }
        file << this->transfer[i] << " ";


    }
    file << "\n";

}


