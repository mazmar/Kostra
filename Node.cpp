/* 
 * File:   Node.cpp
 * Author: mazmart
 * 
 * Created on November 20, 2012, 11:36 PM
 */

#include "Node.h"
#include "Kostra.h"

template<> Node<Kostra>::Node(Kostra * k) {
    ///TODO: dokoncit kopy!!;
    //        this->k = Kostra(k->kostra);
    //        this->k = new Kostra(k);
    this->k = k;
    this->prev = NULL;
    this->next = NULL;
}

template<> void Node<Kostra>::addChild(Kostra * kostra) {
    Node * n = new Node(kostra);
    if (this->firstChild == NULL){
        this->firstChild = n;
        this->lastChild = this->firstChild;
    }else{
        this->lastChild->next = n;
        n->prev = this->lastChild;
        this->lastChild = n;
    }
}
