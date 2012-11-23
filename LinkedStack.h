/* 
 * File:   LinkedStack.h
 * Author: mazmart
 *
 * Created on November 18, 2012, 1:39 PM
 */



#ifndef LINKEDSTACK_H
#define	LINKEDSTACK_H
#include "Node.h"

template <class K>
class LinkedStack {
public:
    Node<K> * front;
    Node<K> * back;
    Node<K> * middle;
    int size;

    LinkedStack();
    ~LinkedStack();

    void add(Node<K> * x);
    void add(K * x);
    void add(LinkedStack<K>*);

    void moveUp();
    void moveUpChildren();

    K * next();
    K * popBack();
    K * popFront();
    LinkedStack * divide();

    void print();
    bool isEmpty();
    void recalculateMiddle();
};

#endif	/* LINKEDSTACK_H */