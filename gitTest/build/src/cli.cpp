#include "cli.h"
#include <iostream>

CLI::CLI(VCS& controller) : vcsController(controller) {}

void CLI::parseAndExecute(int argc, char** argv) {
    if (argc < 2) {
        displayUsage();
        return;
    }

    std::string command = argv[1];

    if (command == "init") {
        handleInitCommand();
    } else if (command == "add") {
        if (argc < 3) {
            std::cerr << "Error: No file specified for 'add' command\n";
            displayUsage();
            return;
        }
        handleAddCommand(argv[2]);
    } else if (command == "commit") {
        if (argc < 3) {
            std::cerr << "Error: No commit message specified\n";
            displayUsage();
            return;
        }
        handleCommitCommand(argv[2]);
    } else if (command == "rollback") {
        if (argc < 3) {
            std::cerr << "Error: No commit ID specified for 'rollback' command\n";
            displayUsage();
            return;
        }
        handleRollbackCommand(argv[2]);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        displayUsage();
    }
}

void CLI::handleInitCommand() const {
    vcsController.init();
    std::cout << "Repository initialized." << std::endl;
}

void CLI::handleAddCommand(const std::string& filename) const {
    vcsController.add(filename);
    std::cout << "File added: " << filename << std::endl;
}

void CLI::handleCommitCommand(const std::string& message) const {
    vcsController.commit(message);
    std::cout << "Changes committed with message: " << message << std::endl;
}

void CLI::handleRollbackCommand(const std::string& commitId) const {
    vcsController.revert(commitId);
    std::cout << "Rolled back to commit: "  << std::endl;
}

void CLI::displayUsage() const {
    std::cout << "Usage: MiniVCS [command] [arguments]\n"
              << "Commands:\n"
              << "  init                         Initialize a new repository\n"
              << "  add [file]                   Add a file to the staging area\n"
              << "  commit [message]             Commit changes\n"
              << "  rollback [commit_id]         Rollback to a previous commit\n";
}
