#include "c8.h"
#include "c8_config.h" // CMake configuration file
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

// opengl display
#define SCREEN_W 64
#define SCREEN_H 32
#define MODIFIER 10 // TODO: pull this out to cmake config?

const int window_w = SCREEN_W * MODIFIER;
const int window_h = SCREEN_H * MODIFIER;


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
void opengl_init(int& argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("c8vm");
}
// ----------------------------------------------------------------------------

void draw_buf(const byte* gfx_buf, const unsigned int size) {
    return;
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

    // opengl stuff (window, input callbacks)
    opengl_init(argc, argv);

    // init vm
    C8VM vm;
    vm.load(bin);
    vm.start();

    return 0;
}
