#include "../include/VCSController.h"
#include "../include/CLI.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    VCSController vcsController; // Create the VCS controller
    CLI CLI(vcsController);

    CLI.parseAndExecute(argc, argv); // Parse and execute the command-line arguments

    return 0;
}


