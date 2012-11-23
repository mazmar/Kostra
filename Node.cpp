/* 
 * File:   Node.cpp
 * Author: mazmart
 * 
 * Created on November 20, 2012, 11:36 PM
 */

#include "Node.h"
#include "Kostra.h"
#include <iostream>




//// 100 
//7 9 12 15 -1
//0 0 0  0  -1
//0 1 2  2  -1
//        
//        
//        
//        //  0  1  2  3  4  5 6  7 8  9 10 11 12 13 14
//           -1 -1 -1 -1 -1 -1 0 -1 0 -1 -1  0 -1 -1  0
//           -1 -1 -1 -1 -1 -1 0 -1 1 -1 -1  2 -1 -1  2
//        



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

template<> Node<Kostra>::~Node() {
    if (this->prev != NULL)this->prev->next = this->next;
    if (this->next != NULL)this->next->prev = this->prev;
    this->prev = NULL;
    this->next = NULL;
    if (this->firstChild != this->lastChild)
        delete this->lastChild;
    delete this->firstChild;
    this->parent = NULL;
}



template<> int Node<Kostra>::idc = 1;
