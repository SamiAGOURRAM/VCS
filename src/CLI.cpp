#include "../include/CLI.h"
#include <iostream>


using namespace std;
CLI::CLI(VCSController& controller) : vcsController(controller) {}

void CLI::parseAndExecute(int argc, char** argv) {
    if (argc < 2) {
        cout<<"I am here";
        displayUsage();
        return;
    }

    string command = argv[1];

    if (command == "init") {
        handleInitCommand();
    } else if (command == "add") {
        if (argc < 3) {
            cerr << "Error: No file specified for 'add' command\n";
            displayUsage();
            return;
        }
        handleAddCommand(argv[2]);
    } else if (command == "commit") {
        if (argc < 3) {
            cerr << "Error: No commit message specified\n";
            displayUsage();
            return;
        }
        handleCommitCommand(argv[2]);
    } else if (command == "rollback") {
        if (argc < 3) {
            cerr << "Error: No commit ID specified for 'rollback' command\n";
            displayUsage();
            return;
        }
        handleRollbackCommand(argv[2]);
    } else {
        cerr << "Unknown command: " << command << endl;
        displayUsage();
    }
}

void CLI::handleInitCommand() const {
    vcsController.init(".");
    cout << "Repository initialized." << endl;
}

void CLI::handleAddCommand(const string& filename) const {
    vcsController.add(filename);
    cout << "File added: " << filename << endl;
}

void CLI::handleCommitCommand(const string& message) const {
    vcsController.commit(message);
    cout << "Changes committed with message: " << message << endl;
}

void CLI::handleRollbackCommand(const string& commitId) const {
    vcsController.rollback(commitId);
    cout << "Rolled back to commit: " << commitId << endl;
}

void CLI::displayUsage() const {
    cout << "Usage: MiniVCS [command] [arguments]\n"
              << "Commands:\n"
              << "  init                         Initialize a new repository\n"
              << "  add [file]                   Add a file to the staging area\n"
              << "  commit [message]             Commit changes\n"
              << "  rollback [commit_id]         Rollback to a previous commit\n";
}
