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

#define WORK_REQUEST 0
//posílání práce
#define WORK_TRANSMIT 1
//posílání řešení
#define SOLUTION_TRANSMIT 2
//posílání příznaku o ukončení práce
#define END_WORK 3
//zaslání peška
#define TOKEN 4

#define WHITE 0
#define BLACK 1

#define KILLSWITCH = 1;



using namespace std;

const int NO_NEXT = 20;

const int VERTICES = 15;

const int REQUESTTRESHOLD = 20;

int requestCount = 0;
int worldSize;
int rank;
int reqRank = 0;
int workRequested = 0;
Kostra * solution = NULL;
LinkedStack<Kostra> solutions;
MPI_Status status;
Transfer * stackTransfer = NULL;
MPI_Request stackTransferRequest;

int token;
int tokenColor = BLACK;
int processColor = WHITE;
int sendingBuffer = 0;

int endWork = 0;


int workRequestDataLength = 10;
char workRequestData[10] = "More work";
short emptyBuffer = 1;


int graf[VERTICES * VERTICES];

Uzel * uzly;

LinkedStack<Kostra> * stack = NULL;

void checkRequests() {
    for (int i = 0; i < worldSize; i++) {
        if (i == rank) continue;
        cout << "Process " << rank << " : Checking Requests from:" << i;
        int flag;
        MPI_Iprobe(i, WORK_REQUEST, MPI_COMM_WORLD, &flag, &status);
        cout << " Flag:" << flag << "\n";
        if (flag > 0) {
            // prijata zadost o praci

            cout << "Process " << rank << " : Cekam na odeslani predosli spravy\n";
            if (sendingBuffer) {
                MPI_Wait(&stackTransferRequest, &status);
            }
            cout << "Process " << rank << " : Posilam spravu:" << i << "\n";
            MPI_Recv(workRequestData, workRequestDataLength, MPI_CHAR, i, WORK_REQUEST, MPI_COMM_WORLD, &status);
            cout << "Process " << rank << " : Prijimam zadost:" << i << "\n";

            if (stack == NULL) {
                stack = new LinkedStack<Kostra > ();
            }
            if (stackTransfer != NULL) {
                delete stackTransfer;
                stackTransfer = NULL;
            }
            cout << "Process " << rank << " : Odesilam:" << stack->size << "\n";

            if (!stack->isEmpty()) {
                stackTransfer = new Transfer(stack->divide());
                cout << "Process " << rank << " : Transfer vytvoren:" << stackTransfer->size << "\n";

                MPI_Isend(stackTransfer->transfer, stackTransfer->size, MPI_SHORT, i, WORK_TRANSMIT, MPI_COMM_WORLD, &stackTransferRequest);
                sendingBuffer = 1;
            } else {
                MPI_Send(&emptyBuffer, 1, MPI_SHORT, i, WORK_TRANSMIT, MPI_COMM_WORLD);

            }
            cout << "Process " << rank << " : Odeslano:" << i << "\n";

            //zahravky s peskom
            if (stackTransfer != NULL && rank > i) {
                processColor = BLACK;
            }
        }
    }
}

void checkToken() {
    int process = ((rank - 1) + worldSize) % worldSize;
    int flag;
    MPI_Iprobe(process, TOKEN, MPI_COMM_WORLD, &flag, &status);
    cout << "Process " << rank << " : Posloucha peska :" << process << " flag:" << flag << "\n";

    if (flag) {
        MPI_Recv(&tokenColor, 1, MPI_INT, process, TOKEN, MPI_COMM_WORLD, &status);
        token = 1;
        cout << "Process " << rank << " : Prijma peska :" << process << "o barve" << tokenColor << "\n";

        if (processColor == BLACK) {
            tokenColor = BLACK;
        }

    }


    if (rank == 0 && token == 1 && tokenColor == WHITE) {
        int end = 1;
        for (int i = 0; i < worldSize; i++) {
            cout << "Process " << rank << " : Sending KILL\n";
            MPI_Send(&end, 1, MPI_INT, i, END_WORK, MPI_COMM_WORLD);

        }
        endWork = 1;
    }
}

void setSolution(Kostra * kostra) {
    cout << "Process 0 : Setting solution " << kostra->krok << " and deleting stack\n";

    if (solution == NULL || solution->krok > kostra->krok) {
        solution = kostra;
        delete stack;
        stack = new LinkedStack<Kostra>();
    }
}

void sendSolution() {
    cout << "Process 0 : Sending Solutions\n";
    LinkedStack<Kostra> tempStack;
    tempStack.add(solution);
    solution->toString();
    Transfer * t = new Transfer(&tempStack);
    for (int i = 0; i < worldSize; i++) {
        //neposilej zpravu sam sobe
        if (i == rank) continue;
        MPI_Send(t->transfer, t->size, MPI_SHORT, i, SOLUTION_TRANSMIT, MPI_COMM_WORLD);
    }
    t->print();
    delete t;
}

void checkSolution() {
    LinkedStack<Kostra> tempStack;
    int flag, testTransferSize;
    for (int i = 0; i < worldSize; i++) {
        if (i == rank)continue;
        MPI_Iprobe(i, SOLUTION_TRANSMIT, MPI_COMM_WORLD, &flag, &status);
        cout << "\nProcess " << rank << " : Checking Solutions from: " << i << "flag:" << flag << "\n";

        if (flag) {
            MPI_Get_count(&status, MPI_SHORT, &testTransferSize);
            Transfer * t = new Transfer(testTransferSize);
            cout << "Prijmam solution o velkosti: " << testTransferSize << "\n";
            MPI_Recv(t->transfer, testTransferSize, MPI_SHORT, i, SOLUTION_TRANSMIT, MPI_COMM_WORLD, &status);
            cout << "Prijal sem solution o velkosti: " << testTransferSize << "\n";

            tempStack = *t->unpack();
            if (solution == NULL || tempStack.front->k->krok < solution->krok)
                setSolution(tempStack.front->k);
            delete t;
        }
        
    }
    cout << "\nProcess " << rank << " : Exiting Solutions: ";

}

void checkEnd() {
    MPI_Iprobe(0, END_WORK, MPI_COMM_WORLD, &endWork, &status);
}

void checkMsgs() {
    if (requestCount >= REQUESTTRESHOLD) {
        checkEnd();
        checkToken();
        checkRequests();
        checkSolution();
        // check token and solution
        requestCount = 0;
    }
    requestCount++;
}

void requestWork() {
    //TODO: Pesek a procesColor
    if (token) {
        int process = (rank + 1) % worldSize;
        if (rank == 0) {
            tokenColor = WHITE;
        }
        MPI_Send(&tokenColor, 1, MPI_INT, process, TOKEN, MPI_COMM_WORLD);
        token = 0;
        cout << "Process " << rank << " : Posila peska:" << process << "\n";

    }
    processColor = WHITE;
    cout << "Process " << rank << " : Request Work from :" << reqRank << " a mam peska:" << token << "\n";

    MPI_Send(&workRequestData, workRequestDataLength, MPI_CHAR, reqRank, WORK_REQUEST, MPI_COMM_WORLD);
}

bool checkWork() {
    int flag;

    MPI_Iprobe(reqRank, WORK_TRANSMIT, MPI_COMM_WORLD, &flag, &status);
    cout << " Flag:" << flag << "\n";
    if (flag) {
        int testTransferSize;
        MPI_Get_count(&status, MPI_SHORT, &testTransferSize);

        cout << "\nTransfer Size: " << testTransferSize << "\n";
        if (testTransferSize == 1)return false;
        Transfer * transfer = new Transfer(testTransferSize);

        MPI_Recv(transfer->transfer, testTransferSize, MPI_SHORT, reqRank, WORK_TRANSMIT, MPI_COMM_WORLD, &status);
        cout << "Process " << rank << " Prijal spravu o velkosti:" << testTransferSize << "\n";

        stack->add(transfer->unpack());
        // free(transfer);
    }
    return flag == 0;
}

void mainCycle() {
    while (true) {
        checkMsgs();

        if (endWork)break;

        if (workRequested) {
            cout << "Process " << rank << ": Checking Work Transmits from: " << reqRank;
            workRequested = checkWork();
            if (workRequested) {
                continue;
            }
        }

        if (stack == NULL || stack->isEmpty()) {
            reqRank = (reqRank + 1) % worldSize;
            if (rank == reqRank) reqRank = (reqRank + 1) % worldSize;
            requestWork();
            workRequested = true;
            //            
            //            if (reqRank == rank) reqRank++;

        } else {
            Kostra * first = stack->front->k;
            //do not continue if solution is already smaller

            Kostra * next = stack->next();
            //            setSolution()
            if (next->kostra.size() == VERTICES) {
                if (solution == NULL || solution->krok > next->krok) {
                    cout << "Setting solution: " << next->krok << "\n";
                    setSolution(next);
                    sendSolution();
                }
            }
        }
    }


    cout << "Process " << rank << ": ";
//    if (stack != NULL)stack->print();
    if (solution != NULL) {
        cout << "Process " << rank << ": Solution ";
        solution->toString();
    }





}

int main(int argc, char ** argv) {
    int graf[VERTICES * VERTICES] = {
        0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1,
        0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0,
        1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0
    };


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    cout << "After init\n";
    stack = new LinkedStack<Kostra > ();
    uzly = new Uzel[VERTICES];
    for (int i = 0; i < VERTICES; i++) {
        for (int j = 0; j < VERTICES; j++) {
            if (graf[i * VERTICES + j] == 1) {
                uzly[i].hrany.push_back(uzly[j]);
            }
        }
    }
    //    
    if (rank == 0) {
        // predist pytaniu sameho seba
        reqRank++;

        // uvodny uzel do stack
        vector<Uzel> kostra;
        kostra.push_back(uzly[6]);
        Kostra * k = new Kostra(kostra);
        stack->add(k);
    }
    // TEST
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    stack->next();
    //    //    stack->print();


    token = (rank == 0) ? 1 : 0;
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << " : Starting cycle\n";
    mainCycle();


    cout << "Process " << rank << ": Ukoncen???";
    MPI_Finalize();
    return 0;
}



//
/////TEST
//    int * rmsg = 0;
// cout << "Rank: " << rank << " Size: " << worldSize << "\n";
//    cout << "Before recieving Msg: " << *rmsg << "\n";
//
//    MPI_Send(&graf, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);
//    MPI_Recv(&rmsg, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
//
//    cout << "Recieved Msg: " << *rmsg;
//
//    uzly = new Uzel[8];
//
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            if (graf[i * 8 + j] == 1) {
//                uzly[i].hrany.push_back(uzly[j]);
//            }
//        }
//    }
//    vector<Uzel> kostra;
//    kostra.push_back(uzly[6]);
//    Kostra * k = new Kostra(kostra);
//
//
//    k->toString();
//    //test stack
//    stack = new LinkedStack<Kostra > ();
//    cout << "\nSize of linked Stack: " << stack->size;
//
//
//    //    Node * x = new Node(k);
//    stack->add(k);
//    stack->back->k->toString();
//
//    cout << "\nSize of linked Stack: " << stack->size;
//
//
//    kostra.push_back(uzly[5]);
//    cout << "\n------Next of Front then Front----------------\n";
//    Node<Kostra> * temp = stack->front;
//    k = temp->k->next();
//
//    //    x = new Node(k);
//    cout << "\n------Front then add----------------\n";
//    stack->add(k);
//
//    cout << "\n------Prev of Back and Back----------------\n";
//
//
//    temp = stack->front;
//    temp->k->toString();
//    k = temp->k->next();
//
//    stack->add(k);
//    temp = stack->front;
//    temp->k->toString();
//    k = temp->k->next();
//    stack->add(k);
//
//    temp = stack->front;
//    temp->k->toString();
//    k = temp->k->next();
//    stack->add(k);
//
//    temp = stack->front;
//    temp->k->toString();
//    k = temp->k->next();
//    stack->add(k);
//
//    temp = stack->front;
//    temp->k->toString();
//    k = temp->k->next();
//    stack->add(k);
//
//    cout << "\n------Komplet Stack Before split----------------\n";
//    Node<Kostra> * n = stack->front;
//    while (n != NULL) {
//        n->k->toString();
//        n = n->next;
//    }
//    //
//    //
//    LinkedStack<Kostra> * second = stack->divide();
//    //
//    //
//    cout << "\n------Komplet Stack after Split----------------\n";
//    n = stack->front;
//    while (n != NULL) {
//        n->k->toString();
//        n = n->next;
//    }
//    //
//    cout << "\n------Komplet Second after Split----------------\n";
//    n = second->front;
//    while (n != NULL) {
//        n->k->toString();
//        n = n->next;
//    }
//    //
//
//
//    //    Transfer * t = new Transfer(stack->divide());
//
//    Transfer t(second);
//    t.print();
//    cout << "\n------SEND MPI----------------\n";
//    MPI_Send(t.transfer, t.size, MPI_SHORT, 0, 0, MPI_COMM_WORLD);
//    //
//    //
//    int testTransferSize;
//    int flag;
//    MPI_Iprobe(0, MPI_ANY_SOURCE, MPI_COMM_WORLD, &flag, &status);
//    MPI_Get_count(&status, MPI_SHORT, &testTransferSize);
//    //
//    cout << "\nTransfer Size: " << testTransferSize << "\n";
//    //    short * testTransfer = new short[testTransferSize];
//    Transfer* newTransfer = new Transfer(testTransferSize);
//    MPI_Recv(newTransfer->transfer, testTransferSize, MPI_SHORT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//    delete second;
//    //
//
//    //
//    cout << "\n------TransferedStack----------------\n";
//    LinkedStack<Kostra> * trasferedStack = newTransfer->unpack();
//    n = trasferedStack->front;
//    while (n != NULL) {
//        n->k->toString();
//        n = n->next;
//    }
//    stack->add(trasferedStack);
//
//    cout << "\n------Komplet stack after merge----------------\n";
//    n = stack->front;
//    while (n != NULL) {
//        n->k->toString();
//        n = n->next;
//    }
//
//    //    
//    //    temp = stack->front;
//    //    k = temp->k->next();
//    //    stack->add(k);
//
//
//
//    cout << "\nSize of linked Stack: " << stack->size << "\n";
//    //
//    //
//    //    // test stack children
//    //
//    //    stack->front->k.toString();
//
//
//
//    MPI_Finalize();
//    return 0;
//
//
//
//
//    // 1 2 3 4 5 6 7 8
//    //    
//    //
//    //    vector<Uzel> kostra;
//    //    deque<Kostra> fulls;
//    //    kostra.push_back(uzly[6]);
//    //    Kostra k(kostra, uzly);
//    //
//    //    stack->push_front(k);
//    //    int x = 0;
//    //    while (!stack->empty()) {
//    //        if (next->isFull()) {
//    //            fulls.push_front(*next);
//    //        } else {
//    //            stack->push_back(*next);
//    //        }
//    //        x++;
//    //    }
//    //
//    //    cout << "Stack\n";
//    //    for (deque<Kostra>::iterator i = stack->begin(); i != stack->end(); i++) {
//    //        i->toString();
//    //    }
//    //
//    //    cout << "Fulls\n";
//    //    //    for (deque<Kostra>::iterator it = fulls.begin(); it != fulls.end(); it++) {
//    //    ////        it->toString();
//    //    ////        cout << "7\n" << it->output;
//    //    //
//    //    //    }
//    //    deque<Kostra>::iterator it = fulls.end();
//    //    it--;
//    //    it--;
//    //    cout << "7\n" << it->output;
//
//
//
//}
//
//    cout << "TEST ANYSEK\n";
//    
//    stack->add(k);
//    Node<Kostra> * temp = stack->front;
//    cout << "\n\n-----Adresa" << stack->front << "\n\n";
//    stack->next();
//    stack->next();
//    stack->next();
//    stack->next();
//    stack->next();
//    stack->next();
//    stack->next();
//    stack->print();
//     stack->popFront();
//    
////     cout << "\n\n\n\nTEST ANYSEK\n";
//////     cout << "\n\n-----Pocet Deti: " << temp->id << "\n\n";
////    stack->print();
////    Node<Kostra> * t = new Node(k);
////    delete();
////    k->toString();
//    stack->print();
//
//
//    MPI_Finalize();
//    return 0;
//}







