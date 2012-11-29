/* 
 * File:   LinkedStack.cpp
 * Author: mazmart
 * 
 * Created on November 18, 2012, 1:39 PM
 */
#include <iostream>
#include "LinkedStack.h"
#include "Kostra.h"
#include "Node.h"

using namespace std;

template<> LinkedStack<Kostra>::LinkedStack() {
    this->size = 0;
    this->front = NULL;
    this->back = this->front;
}

template<> LinkedStack<Kostra>::~LinkedStack() {
    file << "deleting stack\n";
    //    if (this->front!=this->back)delete this->back;
    //    delete this->front;
    this->middle = NULL;
    this->back = NULL;
    this->front = NULL;
    this->size = 0;
    file << "delete OK\n";

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

template<> void LinkedStack<Kostra>::print() {
    file << "Current stack:\n";
    Node<Kostra> * n = this->front;
    while (n != NULL || n != (Node<Kostra>*)0) {
        n->print();
        n = n->next;
    }
    file << "Stack size " << this->size << "\n";
}

template<> bool LinkedStack<Kostra>::isEmpty() {
    return (this->front == NULL || this->size == 0);
}

template<> void LinkedStack<Kostra>::recalculateMiddle() {
    this->middle = this->front;
    int m = ((this->size - 1) / 2);
    for (int i = 0; i < m; i++) {
        this->middle = this->middle->next;
    }
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
        this->back = NULL;
    } else {
        this->front = temp->next;
    }
    this->size--;
    Kostra * r = temp->k;
    //    delete temp;
    return r;
}

template<> void LinkedStack<Kostra>::moveUpChildren() {
    Node<Kostra> * temp = this->front->firstChild;
    if (temp == NULL) return;
    this->front->next = temp;
    temp->prev = this->front;
    this->back = this->front->lastChild;

    while (temp != NULL) {
        temp->parent = NULL;
        temp = temp->next;
    }
    this->front->firstChild = NULL;
    this->front->lastChild = NULL;
    this->size += this->front->childSize;
    this->front->childSize = 0;
}

template<> void LinkedStack<Kostra>::moveUp() {

    Node<Kostra> * temp = this->front;
    file << "move Up" << temp->next << " " << " " << temp->childSize << "\n";
    if (temp->childSize == 0) {
        file << "childSize0" << temp << "\n";
        this->popFront();
        return;
    } else if (this->front->next == NULL) {
        file << "nextNULL" << temp << "\n";
        this->front = this->front->firstChild;

    } else {
        file << "NormalMoveUp" << temp << "\n";
        this->front = temp->next;
        this->front->prev = NULL;
        this->back->next = temp->firstChild;

        temp->firstChild->prev = this->back;
    }
    this->back = temp->lastChild;
    Node<Kostra> * child = temp->firstChild;
    while (child != NULL) {
        child->parent = temp->parent;
        child = child->next;
    }
    temp->firstChild = NULL;
    temp->lastChild = NULL;
    this->size += temp->childSize - 1;
    // smazat to co tem odkazuje
    //    delete(temp);
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
    if (this->size == 1)this->moveUpChildren();
    return x;
}

template<> Kostra * LinkedStack<Kostra>::next() {
    Kostra * kostra = this->front->k->next();
    file << "TEST " << kostra;
    if (kostra == NULL) {
        this->moveUp();
    } else {
        file << "nok\n";
        this->front->addChild(kostra);
    }
    return kostra;
}

