/* 
 * File:   Uzel.cpp
 * Author: mazmart
 * 
 * Created on November 18, 2012, 1:58 PM
 */
#include <iostream>
#include <list>
#include "Uzel.h"

using namespace std;

const int NO_NEXT = 0;

void Uzel::toString() {
    cout << "Uzel " << this->name + 1 << " " << current << " [ ";
    for (list<Uzel>::iterator i = hrany.begin(); i != hrany.end(); i++) {
        cout << i->name + 1 << " ";
    }
    cout << "] " << "pridany v " << pridane << "\n";
}

Uzel::Uzel() {
    current = 0;
    pridane = 0;
    name = n++;
}

void Uzel::remove(Uzel x) {
    hrany.remove(x);
}

bool Uzel::operator==(Uzel x) {
    return name == x.name;
}

Uzel Uzel::next() {
    if (current >= hrany.size()) {
        throw NO_NEXT;
    }
    list<Uzel>::iterator it = hrany.begin();
    for (int i = 0; i < current; i++) {
        it++;
        if (it == hrany.end()) return *it;
    }
    current++;
    return *it;
}

bool Uzel::moveToNext() {
    this->current++;
    if (this->current < this->hrany.size()) {
        return false;
    } else {
        return true;
    }
}


int Uzel::n = 0;



