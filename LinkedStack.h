/* 
 * File:   LinkedStack.h
 * Author: mazmart
 *
 * Created on November 18, 2012, 1:39 PM
 */

#ifndef LINKEDSTACK_H
#define	LINKEDSTACK_H

template <class T>
class Node {
public:
    T * k;
    Node * prev;
    Node * next;

    Node(T * k);
};

template <class T>
class LinkedStack {
public:
    Node<T> * front;
    Node<T> * back;
    Node<T> * middle;
    int size;

    LinkedStack();

    void add(Node<T> * x);
    void add(T * x);
    void add(LinkedStack<T>*);
    T * popBack();
    T * popFront();
    LinkedStack * divide();

    void recalculateMiddle() {
        this->middle = this->front;
        int m = ((this->size - 1) / 2);
        for (int i = 0; i < m; i++) {
            this->middle = this->middle->next;
        }
    }
};



#endif	/* LINKEDSTACK_H */