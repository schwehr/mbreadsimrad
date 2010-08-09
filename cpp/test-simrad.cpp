#include "simrad.h"

#include <string>
#include <iostream>
using namespace std;

int main() {
    SimradFile sf(string("../0018_20050728_153458_Heron.all"));

    cout.setf(ios::fixed, ios::floatfield);
    cout.setf(ios::showpoint);
    cout.precision(4);


    for (SimradDg *dg=sf.next(); dg != 0; dg = sf.next()) {
        cout << "datagram: " << dg->getName() << endl;
        delete dg;
    };

    return 0;
}
