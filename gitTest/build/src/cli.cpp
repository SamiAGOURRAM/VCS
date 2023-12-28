#include "cli.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>

CLI::CLI(VCS& controller) : vcsController(controller) {}

void CLI::start() {
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        std::istringstream iss(line);
        std::vector<std::string> args;
        std::string arg;
        bool inQuotes = false;

        // Custom parsing to handle quoted arguments
        while (iss >> std::ws) {  // std::ws to eat up any leading whitespace
            char next = iss.peek();
            if (next == '\"') {
                inQuotes = !inQuotes;
                iss.get();  // Eat the quote
                std::getline(iss, arg, inQuotes ? '\"' : ' ');
                args.push_back(arg);
            } else {
                iss >> arg;
                args.push_back(arg);
            }
        }

        // Convert vector of strings to array of char* for compatibility with parseAndExecute
        std::vector<char*> argv;
        for (auto& arg : args) {
            argv.push_back(&arg.front());
        }

        parseAndExecute(argv.size(), argv.data());
    }
}


void CLI::parseAndExecute(int argc, char** argv) {
    if (argc < 2 || std::string(argv[0]) != "um6p") {
        std::cerr << "Error: All commands must start with 'um6p'\n";
        displayUsage();
        return;
    }

    std::string command = argv[1];

    if (command == "init") {
        if (argc != 2) {
            std::cerr << "Error: 'init' command takes no additional arguments\n";
            displayUsage();
            return;
        }
        handleInitCommand();
    }else if (command == "log"){
        if (argc != 2) {
            std::cerr << "Error: log command takes no additional arguments\n";
            displayUsage();
            return;
        }
        handleLogCommand();

    } else if (command == "add") {
        if (argc != 3) {
            std::cerr << "Error: 'add' command requires exactly one filename argument\n";
            displayUsage();
            return;
        }
        handleAddCommand(argv[2]);
    } else if (command == "commit") {
        if (argc != 3) {
            std::cerr << "Error: 'commit' command requires exactly one message argument\n";
            displayUsage();
            return;
        }
        handleCommitCommand(argv[2]);
    } else if (command == "rollback") {
        if (argc != 3) {
            std::cerr << "Error: 'rollback' command requires exactly one commit ID argument\n";
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

void CLI::handleLogCommand () const{

    vcsController.log();

}
void CLI::displayUsage() const {
    std::cout << "Usage: MiniVCS [command] [arguments]\n"
              << "Commands:\n"
              << "  init                         Initialize a new repository\n"
              << "  add [file]                   Add a file to the staging area\n"
              << "  commit [message]             Commit changes\n"
              << "  rollback [commit_id]         Rollback to a previous commit\n";
}


