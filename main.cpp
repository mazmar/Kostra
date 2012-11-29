/*
 * File:   main.cpp
 * Author: mazmart
 *
 * Created on October 8, 2012, 4:51 PM
 */

#include <iostream>
#include <fstream>
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


ofstream file;

void checkRequests() {
    for (int i = 0; i < worldSize; i++) {
        if (i == rank) continue;
        file << "Process " << rank << " : Checking Requests from:" << i;
        int flag;
        MPI_Iprobe(i, WORK_REQUEST, MPI_COMM_WORLD, &flag, &status);
        file << " Flag:" << flag << "\n";
        if (flag > 0) {
            // prijata zadost o praci

            file << "Process " << rank << " : Cekam na odeslani predosli spravy\n";
            if (sendingBuffer) {
                MPI_Wait(&stackTransferRequest, &status);
            }
            file << "Process " << rank << " : Posilam spravu:" << i << "\n";
            MPI_Recv(workRequestData, workRequestDataLength, MPI_CHAR, i, WORK_REQUEST, MPI_COMM_WORLD, &status);
            file << "Process " << rank << " : Prijimam zadost:" << i << "\n";

            if (stack == NULL) {
                stack = new LinkedStack<Kostra > ();
            }
            if (stackTransfer != NULL) {
                delete stackTransfer;
                stackTransfer = NULL;
            }
            file << "Process " << rank << " : Odesilam:" << stack->size << "\n";

            if (!stack->isEmpty()) {
                stackTransfer = new Transfer(stack->divide());
                file << "Process " << rank << " : Transfer vytvoren:" << stackTransfer->size << "\n";

                MPI_Isend(stackTransfer->transfer, stackTransfer->size, MPI_SHORT, i, WORK_TRANSMIT, MPI_COMM_WORLD, &stackTransferRequest);
                sendingBuffer = 1;
            } else {
                MPI_Send(&emptyBuffer, 1, MPI_SHORT, i, WORK_TRANSMIT, MPI_COMM_WORLD);

            }
            file << "Process " << rank << " : Odeslano:" << i << "\n";

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
    file << "Process " << rank << " : Posloucha peska :" << process << " flag:" << flag << "\n";

    if (flag) {
        MPI_Recv(&tokenColor, 1, MPI_INT, process, TOKEN, MPI_COMM_WORLD, &status);
        token = 1;
        file << "Process " << rank << " : Prijma peska :" << process << "o barve" << tokenColor << "\n";

        if (processColor == BLACK) {
            tokenColor = BLACK;
        }

    }


    if (rank == 0 && token == 1 && tokenColor == WHITE) {
        int end = 1;
        for (int i = 0; i < worldSize; i++) {
            file << "Process " << rank << " : Sending KILL\n";
            MPI_Send(&end, 1, MPI_INT, i, END_WORK, MPI_COMM_WORLD);

        }
        endWork = 1;
    }
}

void setSolution(Kostra * kostra) {
    file << "Process 0 : Setting solution " << kostra->krok;

    if (solution == NULL || solution->krok > kostra->krok) {
        solution = kostra;
        file << "Moj krok " << stack->front->k->krok;
        if (stack->front->k->krok - 1 >= kostra->krok) {
            file << " and deleting stack\n";
            delete stack;
            stack = new LinkedStack<Kostra > ();
        } else {
            file << "and notDeleting Stack\n";
        }
    }
}

void sendSolution() {
    file << "Process 0 : Sending Solutions\n";
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
        file << "\nProcess " << rank << " : Checking Solutions from: " << i << "flag:" << flag << "\n";

        if (flag) {
            MPI_Get_count(&status, MPI_SHORT, &testTransferSize);
            Transfer * t = new Transfer(testTransferSize);
            file << "Prijmam solution o velkosti: " << testTransferSize << "\n";
            MPI_Recv(t->transfer, testTransferSize, MPI_SHORT, i, SOLUTION_TRANSMIT, MPI_COMM_WORLD, &status);
            file << "Prijal sem solution o velkosti: " << testTransferSize << "\n";

            tempStack = *t->unpack();
            if (solution == NULL || tempStack.front->k->krok < solution->krok)
                setSolution(tempStack.front->k);
            delete t;
        }

    }
    file << "\nProcess " << rank << " : Exiting Solutions: ";

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
        file << "\nProcess " << rank << " : Msg Checked \n";
        // check token and solution
        requestCount = 0;
    }
    requestCount++;
}

void requestWork() {
    // zbav sa peska
    if (token) {
        int process = (rank + 1) % worldSize;
        if (rank == 0) {
            tokenColor = WHITE;
        }
        MPI_Send(&tokenColor, 1, MPI_INT, process, TOKEN, MPI_COMM_WORLD);
        token = 0;
        file << "Process " << rank << " : Posila peska:" << process << "\n";

    }
    // nastav sa na bielo
    processColor = WHITE;
    // posli ziadost
    file << "Process " << rank << " : Request Work from :" << reqRank << " a mam peska:" << token << "\n";

    MPI_Send(&workRequestData, workRequestDataLength, MPI_CHAR, reqRank, WORK_REQUEST, MPI_COMM_WORLD);
}

bool checkWork() {
    int flag;

    MPI_Iprobe(reqRank, WORK_TRANSMIT, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        file << "Process " << rank << " Prijma praci\n";
        
        // kontrola velkosti
        int testTransferSize;
        MPI_Get_count(&status, MPI_SHORT, &testTransferSize);

        file << "\nTransfer Size: " << testTransferSize << "\n";
        // prijmanie predoslej ziadosti
        Transfer * transfer = new Transfer(testTransferSize);
        MPI_Recv(transfer->transfer, testTransferSize, MPI_SHORT, reqRank, WORK_TRANSMIT, MPI_COMM_WORLD, &status);

        // pokial 1 == NO_WORK
        if (testTransferSize == 1)return false;

        file << "Process " << rank << " Prijal spravu o velkosti:" << testTransferSize << "\n";
        stack->add(transfer->unpack());
        delete transfer;
    }
    return flag == 0;
}

void mainCycle() {
    while (true) {
        // skontroluj vsetky spravy
        checkMsgs();
        // pokial ziskal KILL koncim
        if (endWork)break;

        // ak nemam pracu a mam poslanu ziadost kontroluj prichod
        if (workRequested) {
            workRequested = checkWork();
            // cyklus pre odoslanie ziadosti len raz
            if (workRequested) {
                continue;
            }
        }

        //pokial je stack prazdny tak posli ziadost reqrank++
        if (stack == NULL || stack->isEmpty()) {

            file << "Process " << rank << ": Stack empty: " << reqRank;

            reqRank = (reqRank + 1) % worldSize;
            if (rank == reqRank) reqRank = (reqRank + 1) % worldSize;
            requestWork();
            workRequested = true;
            //            
            //            if (reqRank == rank) reqRank++;


        } else {
            // nieje prazdny pracuj;

            file << "Process " << rank << ": Stack not Empty: ";
            // zisti ci nemas mensi solution
            file << " krok: " << stack->front->k->krok << " ";
            if (solution) {
                file << " solution: " << solution->krok << " ";
                if ((stack->front->k->krok + 1) >= solution->krok) {
                    file << " Mam Mensi solution vymazavam stack\n";
                    delete stack;
                    stack = new LinkedStack<Kostra > ();
                    continue;
                }
            }


            Kostra * next = stack->next();
            if (next == NULL) continue;

            // nasiel si solution
            if (next->kostra.size() == VERTICES) {
                if (solution == NULL || solution->krok > next->krok) {
                    file << "Setting solution: " << next->krok << "\n";
                    setSolution(next);
                    sendSolution();
                }
            }
        }
    }

    // vypis solution
    file << "Process " << rank << ": ";
    //    if (stack != NULL)stack->print();
    if (solution != NULL) {
        file << "Process " << rank << ": Solution ";
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
    file << "After init\n";
    stack = new LinkedStack<Kostra > ();
    uzly = new Uzel[VERTICES];
    for (int i = 0; i < VERTICES; i++) {
        for (int j = 0; j < VERTICES; j++) {
            if (graf[i * VERTICES + j] == 1) {
                uzly[i].hrany.push_back(uzly[j]);
            }
        }
    }
    string txt = "process";
    txt.append(convertInt(rank));
    txt.append(".txt");

    file.open(txt.c_str());
    file << txt;

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
    //     TEST
    //    Kostra * next = stack->next();
    //               next =  stack->next();
    //               next =  stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //            stack->next();
    //        stack->next();
    //        stack->next();
    //        stack->next();
    //        stack->next();
    //        stack->next();
    //    //    stack->print();


    token = (rank == 0) ? 1 : 0;
    MPI_Barrier(MPI_COMM_WORLD);
    file << "Process " << rank << " : Starting cycle\n";
    mainCycle();


    file << "Process " << rank << ": Ukoncen???";
    //    MPI_Finalize();
    //
    file.close();
    //    return 0;
    //}



    //
    /////TEST
    //    int * rmsg = 0;
    // file << "Rank: " << rank << " Size: " << worldSize << "\n";
    //    file << "Before recieving Msg: " << *rmsg << "\n";
    //
    //    MPI_Send(&graf, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);
    //    MPI_Recv(&rmsg, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    //
    //    file << "Recieved Msg: " << *rmsg;
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
    //        stack->divide();
    //        stack->divide();
    //        Kostra * first = stack->front->k;
    //        Kostra * next = stack->next();
    //            
    //    file << "\nSize of linked Stack: " << stack->size;
    //    stack->print();
    //
    //    //    Node * x = new Node(k);
    //    stack->add(k);
    //    stack->back->k->toString();
    //
    //    file << "\nSize of linked Stack: " << stack->size;
    //
    //
    //    kostra.push_back(uzly[5]);
    //    file << "\n------Next of Front then Front----------------\n";
    //    Node<Kostra> * temp = stack->front;
    //    k = temp->k->next();
    //
    //    //    x = new Node(k);
    //    file << "\n------Front then add----------------\n";
    //    stack->add(k);
    //
    //    file << "\n------Prev of Back and Back----------------\n";
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
    //    file << "\n------Komplet Stack Before split----------------\n";
    //    stack->print();
    //    Node<Kostra> * n = stack->front;
    //    while (n != NULL) {
    //        n->k->toString();
    //        n = n->next;
    //    }
    //    //
    //    //
    //    LinkedStack<Kostra> * second = stack->divide();
    //
    //
    //    file << "\n------Komplet Stack after Split----------------\n";
    //    n = stack->front;
    //    while (n != NULL) {
    //        n->k->toString();
    //        n = n->next;
    //    }
    //    //
    //    file << "\n------Komplet Second after Split----------------\n";
    //    n = second->front;
    //    while (n != NULL) {
    //        n->k->toString();
    //        n = n->next;
    //    }
    //    //
    //
    //
    //        Transfer * t = new Transfer(stack->divide());
    //
    //    Transfer t(second);
    //    t.print();
    //    file << "\n------SEND MPI----------------\n";
    //    MPI_Send(t.transfer, t.size, MPI_SHORT, 0, 0, MPI_COMM_WORLD);
    //    //
    //    //
    //    int testTransferSize;
    //    int flag;
    //    MPI_Iprobe(0, MPI_ANY_SOURCE, MPI_COMM_WORLD, &flag, &status);
    //    MPI_Get_count(&status, MPI_SHORT, &testTransferSize);
    //    //
    //    file << "\nTransfer Size: " << testTransferSize << "\n";
    //    //    short * testTransfer = new short[testTransferSize];
    //    Transfer* newTransfer = new Transfer(testTransferSize);
    //    MPI_Recv(newTransfer->transfer, testTransferSize, MPI_SHORT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    //    delete second;
    //    //
    //
    //    //
    //    file << "\n------TransferedStack----------------\n";
    //    LinkedStack<Kostra> * trasferedStack = newTransfer->unpack();
    //    n = trasferedStack->front;
    //    while (n != NULL) {
    //        n->k->toString();
    //        n = n->next;
    //    }
    //    stack->add(trasferedStack);
    ////
    //    file << "\n------Komplet stack after merge----------------\n";
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
    //    file << "\nSize of linked Stack: " << stack->size << "\n";
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
    //    //    file << "Stack\n";
    //    //    for (deque<Kostra>::iterator i = stack->begin(); i != stack->end(); i++) {
    //    //        i->toString();
    //    //    }
    //    //
    //    //    file << "Fulls\n";
    //    //    //    for (deque<Kostra>::iterator it = fulls.begin(); it != fulls.end(); it++) {
    //    //    ////        it->toString();
    //    //    ////        file << "7\n" << it->output;
    //    //    //
    //    //    //    }
    //    //    deque<Kostra>::iterator it = fulls.end();
    //    //    it--;
    //    //    it--;
    //    //    file << "7\n" << it->output;
    //
    //
    //
    //}
    //
    //    file << "TEST ANYSEK\n";
    //    
    //    stack->add(k);
    //    Node<Kostra> * temp = stack->front;
    //    file << "\n\n-----Adresa" << stack->front << "\n\n";
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
    ////     file << "\n\n\n\nTEST ANYSEK\n";
    //////     file << "\n\n-----Pocet Deti: " << temp->id << "\n\n";
    ////    stack->print();
    ////    Node<Kostra> * t = new Node(k);
    ////    delete();
    ////    k->toString();
    //    stack->print();
    //
    //
    MPI_Finalize();
    return 0;
}







