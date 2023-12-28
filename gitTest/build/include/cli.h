#ifndef CLI_H
#define CLI_H

#include <string>
#include "vcs.h"

class CLI {
public:
    CLI(VCS& controller);
    void start();

    // Parses and executes the given command line arguments
    void parseAndExecute(int argc, char** argv);

private:
    VCS& vcsController;

    // Private helper methods for handling specific commands
    void handleInitCommand() const;
    void handleAddCommand(const std::string& filename) const;
    void handleCommitCommand(const std::string& message) const;
    void handleRollbackCommand(const std::string& commitId) const;

    // Utility method to display usage information
    void displayUsage() const;
};

#endif // CLI_H
