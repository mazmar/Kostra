/*
 * File:   main.cpp
 * Author: mazmart
 *
 * Created on October 8, 2012, 4:51 PM
 */

#include <iostream>
#include <vector>
#include <list>
#include <deque>

using namespace std;

int graf[8 * 8];

class Uzel;

class Kostra;

deque<Kostra> stack;

class Uzel {
public:
    static int n;
    int current;
    int name;
    list<Uzel> hrany;

    void toString() {
        cout << "Uzel " << name + 1 << " " << current << " [ ";
        for (list<Uzel>::iterator i = hrany.begin(); i != hrany.end(); i++) {
            cout << i->name + 1 << " ";
        }
        cout << "]\n";
    }

    Uzel() {
        current = 0;
        name = n++;
    }

    void remove(Uzel x) {
        hrany.remove(x);
    }

    bool operator==(Uzel x) {
        return name == x.name;
    }

    Uzel next() {
        list<Uzel>::iterator it = hrany.begin();
        for (int i = 0; i < current; i++) {
            it++;
            if (it == hrany.end()) return *it;
        }
        current++;
        return *it;
    }

    bool moveToNext() {
        current++;
        if (current < hrany.size()) return false;
        else return true;
    }
};

int Uzel::n = 0;

class Kostra {
    vector<Uzel> kostra;
    Uzel * uzly;
    vector<Uzel>::reverse_iterator current;
public:

    Kostra(vector<Uzel> k, Uzel*& uzly) {
        this->uzly = uzly;
        for (vector<Uzel>::iterator i = k.begin(); i != k.end(); i++) {
            kostra.push_back(uzly[i->name]);
        }
        remove();
        current = kostra.rbegin();
    }

    void remove() {
        for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
            for (vector<Uzel>::iterator it = kostra.begin(); it != kostra.end(); it++) {
                it->remove(*i);
            }
        }
    }

    void remove(Uzel x) {
        for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
            i->remove(x);
        }
    }

    void toString() {
        cout << "Kostra\n";
        for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
            i->toString();
        }
    }

    void add(Uzel x) {
        this->kostra.push_back(uzly[x.name]);
        remove();
    }

    Kostra * next() {
        vector<Uzel> temp = kostra;
        Kostra * k = new Kostra(kostra, uzly);
        for (vector<Uzel>::iterator i = kostra.begin(); i != kostra.end(); i++) {
            k->add(i->next());
        }
        kostra = temp;
        moveToNext();
        return k;
    }

    void test(vector<Uzel>::reverse_iterator it) {
        it->current = 0;
        it++;
        if (it == kostra.rend()) {
            stack.pop_front();
        } else {
            if (it->moveToNext()) test(it);
        }
    }

    bool isFull() {
        return kostra.size() == 8;
    }

    void moveToNext() {
        vector<Uzel>::reverse_iterator i = kostra.rbegin();
        if (i->moveToNext()) test(i);
    }
};

int main(int argc, char ** argv) {
    int graf[8 * 8] = {
        0, 1, 0, 1, 0, 0, 0, 1, // 1
        1, 0, 1, 1, 1, 0, 0, 0, // 2
        0, 1, 0, 0, 1, 1, 0, 0, // 3
        1, 1, 0, 0, 0, 1, 1, 0, // 4
        0, 1, 1, 0, 0, 0, 1, 1, // 5
        0, 0, 1, 1, 0, 0, 1, 0, // 6
        0, 0, 0, 1, 1, 1, 0, 1, // 7
        1, 0, 0, 0, 1, 0, 1, 0 // 8
    };

    // 1 2 3 4 5 6 7 8
    Uzel * uzly = new Uzel[8];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (graf[i * 8 + j] == 1) {
                uzly[i].hrany.push_back(uzly[j]);
            }
        }
    }

    vector<Uzel> kostra;
    deque<Kostra> fulls;
    kostra.push_back(uzly[6]);
    Kostra k(kostra, uzly);

    stack.push_front(k);
    int x = 0;
    while (!stack.empty() && x<40) {
        Kostra * next = stack.front().next();
        if (next->isFull()) {
            fulls.push_front(*next);
        } else {
            stack.push_back(*next);
        }
        x++;
    }

    cout << "Stack\n";
    for (deque<Kostra>::iterator i = stack.begin(); i != stack.end(); i++) {
        i->toString();
    }
    
    cout << "Fulls\n";
    for (deque<Kostra>::iterator it = fulls.begin(); it != fulls.end(); it++) {
        it->toString();
    }

    return 0;
}
