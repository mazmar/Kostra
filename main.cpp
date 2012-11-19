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
#include <mpi.h>
#include "functions.h"
#include "Transfer.h"
#include "Uzel.h"
#include "Kostra.h"
#include "LinkedStack.h"

using namespace std;

const int NO_NEXT = 20;

const int VERTICES = 8;

int graf[VERTICES * VERTICES];

Uzel * uzly;

LinkedStack<Kostra> * stack;

int main(int argc, char ** argv) {
    int rank;
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

    int worldSize;
    int msg = 1000001;
    int rmsg[8 * 8];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);


    cout << "Rank: " << rank << " Size: " << worldSize << "\n";
    cout << "Before recieving Msg: " << *rmsg << "\n";

    MPI_Send(&graf, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Recv(&rmsg, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

    cout << "Recieved Msg: " << *rmsg;

    uzly = new Uzel[8];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (graf[i * 8 + j] == 1) {
                uzly[i].hrany.push_back(uzly[j]);
            }
        }
    }
    vector<Uzel> kostra;
    kostra.push_back(uzly[6]);
    Kostra * k = new Kostra(kostra);


    k->toString();
    //test stack
    stack = new LinkedStack<Kostra>();
    cout << "\nSize of linked Stack: " << stack->size;


    //    Node * x = new Node(k);
    stack->add(k);
    stack->back->k->toString();

    cout << "\nSize of linked Stack: " << stack->size;


    kostra.push_back(uzly[5]);
    cout << "\n------Next of Front then Front----------------\n";
    Node<Kostra> * temp = stack->front;
    k = temp->k->next();

    //    x = new Node(k);
    cout << "\n------Front then add----------------\n";
    stack->add(k);

    cout << "\n------Prev of Back and Back----------------\n";


    temp = stack->front;
    temp->k->toString();
    k = temp->k->next();

    stack->add(k);
    temp = stack->front;
    temp->k->toString();
    k = temp->k->next();
    stack->add(k);

    temp = stack->front;
    temp->k->toString();
    k = temp->k->next();
    stack->add(k);

    temp = stack->front;
    temp->k->toString();
    k = temp->k->next();
    stack->add(k);

    temp = stack->front;
    temp->k->toString();
    k = temp->k->next();
    stack->add(k);

    cout << "\n------Komplet Stack Before split----------------\n";
    Node<Kostra> * n = stack->front;
    while (n != NULL) {
        n->k->toString();
        n = n->next;
    }


    LinkedStack<Kostra> * second = stack->divide();


    cout << "\n------Komplet Stack after Split----------------\n";
    n = stack->front;
    while (n != NULL) {
        n->k->toString();
        n = n->next;
    }

    cout << "\n------Komplet Second after Split----------------\n";
    n = second->front;
    while (n != NULL) {
        n->k->toString();
        n = n->next;
    }


    Transfer t(second);
    t.print();
    cout << "\n------SEND MPI----------------\n";
    MPI_Send(t.transfer, t.size, MPI_INT, 0, 0, MPI_COMM_WORLD);


    int testTransferSize;
    int flag;
    MPI_Iprobe(0, MPI_ANY_SOURCE, MPI_COMM_WORLD, &flag, &status);
    MPI_Get_count(&status, MPI_INT, &testTransferSize);

    cout << "\nTransfer Size: " << testTransferSize << "\n";
    int * testTransfer = new int[testTransferSize];

    MPI_Recv(testTransfer, testTransferSize, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    delete second;

    Transfer newTransfer(testTransfer, &testTransferSize);
    
    cout << "\n------TransferedStack----------------\n";
    LinkedStack<Kostra> * trasferedStack = newTransfer.unpack();
    n = trasferedStack->front;
    while (n != NULL) {
        n->k->toString();
        n = n->next;
    }
    stack->add(trasferedStack);
    
    cout << "\n------Komplet stack after merge----------------\n";
    n = stack->front;
    while (n != NULL) {
        n->k->toString();
        n = n->next;
    }
    
    //    
    //    temp = stack->front;
    //    k = temp->k->next();
    //    stack->add(k);

    

    cout << "\nSize of linked Stack: " << stack->size << "\n";
    //
    //
    //    // test stack children
    //
    //    stack->front->k.toString();



    MPI_Finalize();
    return 0;




    // 1 2 3 4 5 6 7 8
    //    
    //
    //    vector<Uzel> kostra;
    //    deque<Kostra> fulls;
    //    kostra.push_back(uzly[6]);
    //    Kostra k(kostra, uzly);
    //
    //    stack->push_front(k);
    //    int x = 0;
    //    while (!stack->empty()) {
    //        if (next->isFull()) {
    //            fulls.push_front(*next);
    //        } else {
    //            stack->push_back(*next);
    //        }
    //        x++;
    //    }
    //
    //    cout << "Stack\n";
    //    for (deque<Kostra>::iterator i = stack->begin(); i != stack->end(); i++) {
    //        i->toString();
    //    }
    //
    //    cout << "Fulls\n";
    //    //    for (deque<Kostra>::iterator it = fulls.begin(); it != fulls.end(); it++) {
    //    ////        it->toString();
    //    ////        cout << "7\n" << it->output;
    //    //
    //    //    }
    //    deque<Kostra>::iterator it = fulls.end();
    //    it--;
    //    it--;
    //    cout << "7\n" << it->output;

    return 0;
}


