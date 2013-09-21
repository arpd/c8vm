#include "c8.h"
#include "c8_config.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << argv[0] << " version "
             << c8vm_VERSION_MAJOR << "." << c8vm_VERSION_MINOR
             << endl;
        cout << "usage: " << argv[0] << " [[OPT1] [OPT2]] ..." << endl;

    }
    C8VM vm;

    return 0;
}
