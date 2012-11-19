/* 
 * File:   LinkedStack.cpp
 * Author: mazmart
 * 
 * Created on November 18, 2012, 1:39 PM
 */
#include <iostream>
#include "LinkedStack.h"
#include "Kostra.h"

using namespace std;

template<> Node<Kostra>::Node(Kostra * k) {
    ///TODO: dokoncit kopy!!;
    //        this->k = Kostra(k->kostra);
    //        this->k = new Kostra(k);
    this->k = k;
    this->prev = NULL;
    this->next = NULL;
}

template<> LinkedStack<Kostra>::LinkedStack() {
    this->size = 0;
    this->front = NULL;
    this->back = this->front;
}

template<> void LinkedStack<Kostra>::add(LinkedStack<Kostra>* stack) {
    if (this->size == 0) {
        this->front = stack->front;
    } else {
        this->back->next = stack->front;
    }
    this->back = stack->back;
    this->size += stack->size;
    delete stack;
}

template<> void LinkedStack<Kostra>::add(Node<Kostra> * x) {
    if (this->size == 0) {
        this->front = x;
        this->back = x;
    } else {
        x->prev = this->back;
        this->back->next = x;
        this->back = x;
    }
    this->size++;
    if (this->size % 2 == 0) this->middle = this->middle->next;
}

template<> void LinkedStack<Kostra>::add(Kostra * x) {
    if (this->size == 0) {
        this->front = new Node<Kostra > (x);
        this->back = this->front;
    } else {
        this->back->next = new Node<Kostra > (x);
        this->back->next->prev = this->back;
        this->back = this->back->next;
    }
    this->size++;
}

template<> Kostra * LinkedStack<Kostra>::popBack() {
    if (this->size == 0) return NULL;
    Node<Kostra> * temp = this->back;
    if (this->size == 1) {
        this->front = NULL;
        this->back = this->front;
    } else {
        this->back = temp->prev;
    }
    this->size--;
    Kostra * r = temp->k;
    delete temp;
    return r;
}

template<> Kostra * LinkedStack<Kostra>::popFront() {
    if (this->size == 0) return NULL;
    Node<Kostra> * temp = this->front;
    if (this->size == 1) {
        this->front = NULL;
        this->back = this->front;
    } else {
        this->front = temp->next;
    }
    this->size--;
    Kostra * r = temp->k;
    delete temp;
    return r;
}

template<> LinkedStack<Kostra> * LinkedStack<Kostra>::divide() {
    recalculateMiddle();
    LinkedStack * x = new LinkedStack();
    x->front = this->middle->next;
    x->back = this->back;
    this->back = this->middle;
    this->back->next = NULL;
    x->size = this->size / 2;
    this->size = (this->size / 2)+(this->size % 2);
    return x;
}
