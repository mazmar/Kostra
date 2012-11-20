/* 
 * File:   Kostra.cpp
 * Author: mazmart
 * 
 * Created on November 18, 2012, 1:41 PM
 */

#include <iostream>
#include "LinkedStack.h"
#include "Kostra.h"

using namespace std;



Kostra::Kostra(int k=0) {
    krok = k;
    this->hasNext = true;
}

Kostra::Kostra(vector<Uzel> k, int krok, string output) {
    this->krok = krok;
    for (vector<Uzel>::iterator i = k.begin(); i != k.end(); i++) {
        Uzel x = uzly[i->name];
        x.pridane = i->pridane;
        kostra.push_back(x);
    }
    remove();
    current = kostra.rbegin();
    this->output = output;
    this->hasNext = true;
}

void Kostra::remove() {
    for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
        for (vector<Uzel>::iterator it = kostra.begin(); it != kostra.end(); it++) {
            it->remove(*i);
        }
    }
}

void Kostra::remove(Uzel x) {
    for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
        i->remove(x);
    }
}

void Kostra::toString() {
    cout << "\nKostra  krokov " << krok << "\n";
    for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
        i->toString();
    }
}

void Kostra::push(Uzel * u){
    this->kostra.push_back(*u);
    remove();
}

void Kostra::add(Uzel x) {
    Uzel u = uzly[x.name];
    u.pridane = krok;
    this->kostra.push_back(u);
    remove();
}

Kostra * Kostra::next() {
    //        cout << "\n\n---------\n" <<"pred\n";
    //        toString();
    vector<Uzel> temp = kostra;
    if (!this->hasNext) return NULL;
    Kostra * k = new Kostra(kostra, krok, output);
    k->krok++;
    for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
        //            cout << "Removing for "; i->toString(); cout << "\n";
        try {
            Uzel u = i->next();
            k->add(u);
            k->output += convertInt(u.name + 1);
            remove(u);
        } catch (int e) {
            k->output += " ";
        }
    }
    k->output += "\n";
    kostra = temp;
    moveToNext();
    return k;
}


void Kostra::test(vector<Uzel>::reverse_iterator it) {
    it->current = 0;
    it++;
    if (it == kostra.rend()) {
        this->hasNext = false;
    } else {
        if (it->moveToNext()) test(it);
    }
}

bool Kostra::isFull() {
    return kostra.size() == 8;
}

void Kostra::moveToNext() {
    vector<Uzel>::reverse_iterator i = kostra.rbegin();
    if (i->moveToNext()) test(i);
}

