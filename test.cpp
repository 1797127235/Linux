#include <iostream>
#include <unistd.h>  // for sleep
#include<functional>
#include<iostream>
using namespace std;
int main() {

    auto p = [](){
        cout << "hello lambda" << endl;
    };
    p();
    return 0;
}