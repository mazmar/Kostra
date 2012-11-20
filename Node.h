/* 
 * File:   Node.h
 * Author: mazmart
 *
 * Created on November 20, 2012, 11:36 PM
 */

#ifndef NODE_H
#define	NODE_H

template <class T>
class Node {
public:
    T * k;
    Node * parent;
    Node * prev;
    Node * next;
    Node * firstChild;
    Node * lastChild;

    void addChild(T *);
    void getNext();

    Node(T * k);
};

#endif	/* NODE_H */

