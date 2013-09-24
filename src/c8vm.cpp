#include "c8.h"
#include "c8_config.h" // CMake configuration file
#include "debug.h"
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

int window_w = SCREEN_W * MODIFIER;
int window_h = SCREEN_H * MODIFIER;

C8VM vm;

// external key status
byte* keys;

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

void draw_buf(const byte* gfx_buf, const unsigned int size) {
    return;
}
// ----------------------------------------------------------------------------
void key_down(unsigned char key, int x, int y) {
    switch(key) {
        case '1': keys[0x1] = 1; break;
        case '2': keys[0x2] = 1; break;
        case '3': keys[0x3] = 1; break;
        case '4': keys[0xC] = 1; break;
        case 'q': keys[0x4] = 1; break;
        case 'w': keys[0x5] = 1; break;
        case 'e': keys[0x6] = 1; break;
        case 'r': keys[0xD] = 1; break;
        case 'a': keys[0x7] = 1; break;
        case 's': keys[0x8] = 1; break;
        case 'd': keys[0x9] = 1; break;
        case 'f': keys[0xE] = 1; break;
        case 'z': keys[0xA] = 1; break;
        case 'x': keys[0x0] = 1; break;
        case 'c': keys[0xB] = 1; break;
        case 'v': keys[0xF] = 1; break;
    }
}

// ----------------------------------------------------------------------------
void key_up(unsigned char key, int x, int y) {
    switch(key) {
        case '1': keys[0x1] = 0; break;
        case '2': keys[0x2] = 0; break;
        case '3': keys[0x3] = 0; break;
        case '4': keys[0xC] = 0; break;
        case 'q': keys[0x4] = 0; break;
        case 'w': keys[0x5] = 0; break;
        case 'e': keys[0x6] = 0; break;
        case 'r': keys[0xD] = 0; break;
        case 'a': keys[0x7] = 0; break;
        case 's': keys[0x8] = 0; break;
        case 'd': keys[0x9] = 0; break;
        case 'f': keys[0xE] = 0; break;
        case 'z': keys[0xA] = 0; break;
        case 'x': keys[0x0] = 0; break;
        case 'c': keys[0xB] = 0; break;
        case 'v': keys[0xF] = 0; break;
    }
}

// ----------------------------------------------------------------------------
void render(const byte* gfx_buffer) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (gfx_buffer[x + (y*64)] == 0)
                glColor3f(0.0f, 0.0f, 0.0f);
            else
                glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
            glVertex3f((x * MODIFIER) + 0.0f, (y * MODIFIER) + 0.0f,    0.0f);
            glVertex3f((x * MODIFIER) + 0.0f, (y * MODIFIER) + MODIFIER, 0.0f);
            glVertex3f((x * MODIFIER) + MODIFIER, (y * MODIFIER) + MODIFIER, 0.0f);
            glVertex3f((x * MODIFIER) + MODIFIER, (y * MODIFIER) + 0.0f,    0.0f);
            glEnd();
        }
    }
    glutSwapBuffers();
}

// ----------------------------------------------------------------------------
void reshape_window(GLsizei w, GLsizei h)
{
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);

    // Resize quad
    window_w = w;
    window_h = h;
}

// ----------------------------------------------------------------------------
void vm_loop(void) {
    if (vm.is_on()) {
        if (vm.get_gfx_stale()) {
            render(vm.get_gfx_buf());
            vm.set_gfx_stale(true);
        }
        vm.do_cycle();
#if STEPPED
        cout << "cycle completed" << endl;
        cin.get();
#endif
    }
}

// ----------------------------------------------------------------------------
void opengl_init(int& argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("c8vm");

    glutDisplayFunc(vm_loop);
    glutIdleFunc(vm_loop);
    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_up);
    glutReshapeFunc(reshape_window);
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
    keys = vm.get_keys();

    vm.load(bin);
    vm.start();

    glutMainLoop();

    return 0;
}
