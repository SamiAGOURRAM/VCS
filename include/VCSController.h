#ifndef VCS_CONTROLLER_H
#define VCS_CONTROLLER_H

#include <string>
#include <vector>

class VCSController {
public:
    VCSController();
    
    // Initialize a new repository
    void init(const std::string& directory);

    // Add a file to the staging area
    void add(const std::string& filename);

    // Commit changes to the repository
    void commit(const std::string& message);

    // Rollback to a specific commit
    void rollback(const std::string& commitId);

private:
    std::string currentDirectory;
    std::vector<std::string> stagedFiles;

    // Helper functions
    bool isRepositoryInitialized() const;
    void saveCommit(const std::string& message);
    void loadCommit(const std::string& commitId);
};

#endif // VCS_CONTROLLER_H
