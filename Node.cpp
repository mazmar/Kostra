/* 
 * File:   Node.cpp
 * Author: mazmart
 * 
 * Created on November 20, 2012, 11:36 PM
 */

#include "Node.h"
#include "Kostra.h"
#include <iostream>

using namespace std;

template<> Node<Kostra>::Node(Kostra * k) {
    ///TODO: dokoncit kopy!!;
    //        this->k = Kostra(k->kostra);
    //        this->k = new Kostra(k);
    this->k = k;
    this->prev = NULL;
    this->next = NULL;
    this->firstChild = NULL;
    this->lastChild = NULL;
    this->id = idc++;
    this->parent = NULL;
    this->childSize = 0;
}

template<> void Node<Kostra>::addChild(Kostra * kostra) {
    Node * n = new Node(kostra);
    n->parent = this;
    if (this->firstChild == NULL) {
        this->firstChild = n;
        this->lastChild = this->firstChild;
    } else {
        this->lastChild->next = n;
        n->prev = this->lastChild;
        this->lastChild = n;
    }
    this->childSize++;
}

template<> void Node<Kostra>::print() {
    cout << "Node id: " << this->id;
    if (this->parent != NULL)cout << " parent: " << this->parent->id;
    cout << " children: " << this->childSize;
    this->k->toString();
    Node * n = this->firstChild;
    while (n != NULL) {
        n->print();
        n = n->next;
    }
}



template<> int Node<Kostra>::idc = 1;
