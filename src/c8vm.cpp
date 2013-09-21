#include "c8.h"
#include "c8_config.h" // CMake configuration file
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// ----------------------------------------------------------------------------
void print_usage() {
    string prog("c8vm");
    cout << prog << " version "
         << c8vm_VERSION_MAJOR << "." << c8vm_VERSION_MINOR
         << endl;
    cout << "usage: " << prog << " [[OPT1] [OPT2]] ..." << endl;
}

// ----------------------------------------------------------------------------
string& load_binary(string& out, char* path) {
    ifstream infs(path, ios::binary);
    if (infs) {
        /* get filesize so we don't have to read char by char */
        infs.seekg(0, ios::end);
        streampos file_len = infs.tellg();
        infs.seekg(0, ios::beg);

        /* use a vector as the buffer, as they are exception safe and will
           be tidied up correctly */
        vector<char> buf(file_len);
        infs.read(&buf[0], file_len);
        out = string(buf.begin(), buf.end());
    }
    return out;
}

// ----------------------------------------------------------------------------
int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 0;
    }

    // load the binary
    string bin = "";
    bin = load_binary(bin, argv[1]);

    // init vm
    C8VM vm;
    vm.load(bin);
    vm.start();

    return 0;
}
