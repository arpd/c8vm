#include "c8tests.h"
#include "iset.h"
#include "def.h"

#include <iostream>
#include <iomanip>
#include <map>
#include <string>

using namespace std;

map<string, void(*)(vmstate*, c8tests::result*)> tests;

void populate_tests() {
    tests["call_prog"] = c8tests::call_prog;
    tests["clear_screen"] = c8tests::clear_screen;
    tests["ret_routine"] = c8tests::ret_routine;
    tests["jump"] = c8tests::jump;
    tests["call_routine"] = c8tests::jump;
    tests["skip_if_equal"] = c8tests::skip_if_equal;
    tests["skip_if_not_equal"] = c8tests::skip_if_not_equal;
}

void print_result(const c8tests::result& result) {
    cout << "Test: " << result.name << endl;
    cout << "\tpassed: " << boolalpha << result.pass << endl;
    if (!result.pass) {
        cout << "\texpected(" << std::endl << result.expected 
             << endl << "\t)" << endl
             << "\tactual("   << std::endl << result.actual
             << endl << "\t)" << endl;
    }
    cout << "========================================================" << endl;
}

c8tests::result run_test(void (*test_fnc)(vmstate*, c8tests::result*), vmstate* state) {
    c8tests::result r;
    c8tests::clear_result(&r);
    test_fnc(state, &r);
    return r;
}

int main() {
    populate_tests();
    vmstate state;
    int num_tests = 0, num_passes = 0;
    for (auto i = tests.begin(); i != tests.end(); ++i) {
        c8tests::result r(run_test(i->second, &state));
        r.name = i->first;
        ++num_tests;
        if (r.pass) ++num_passes;
        print_result(r);
    }
    cout << num_passes << " out of " << num_tests << " passed." << endl;
}
