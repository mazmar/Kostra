/* 
 * File:   Uzel.h
 * Author: mazmart
 *
 * Created on November 18, 2012, 1:58 PM
 */
#include <list>
#ifndef UZEL_H
#define	UZEL_H

class Uzel {
public:
    static int n;
    int current;
    int name;
    int pridane;
    std::list<Uzel> hrany;

    Uzel();
    
    void toString();
    void remove(Uzel x);
    bool operator==(Uzel x);
    Uzel next();
    bool moveToNext();

private:

};

#endif	/* UZEL_H */

