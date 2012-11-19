
#include <iostream>
#include "functions.h"
using namespace std;

string convertInt(int number) {
    if (number == 0)
        return "0";
    string temp = "";
    string returnvalue = "";
    while (number > 0) {
        temp += number % 10 + 48;
        number /= 10;
    }
    for (int i = 0; i < temp.length(); i++)
        returnvalue += temp[temp.length() - i - 1];
    return returnvalue;
}
