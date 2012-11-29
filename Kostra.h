/* 
 * File:   Kostra.h
 * Author: mazmart
 *
 * Created on November 18, 2012, 1:41 PM
 */

#ifndef KOSTRA_H
#define	KOSTRA_H

#include <vector>
#include <string>
#include <fstream>
#include "Uzel.h"
#include "functions.h"
#include "LinkedStack.h"

using namespace std;

extern Uzel * uzly;
extern ofstream file;


class Kostra {
public:
    std::vector<Uzel> kostra;
    std::vector<Uzel>::reverse_iterator current;
    int krok;
    std::string output;
    bool hasNext;


    Kostra(int k);
    Kostra(vector<Uzel> k, int krok = 0, string output = "");

    void remove();
    void remove(Uzel x);
    void toString();
    void add(Uzel, int);
    Kostra * next();
    void test(vector<Uzel>::reverse_iterator it);

    bool isFull();
    
    void push(Uzel *);

    void moveToNext();

};

extern LinkedStack<Kostra> * stack;

#endif	/* KOSTRA_H */

