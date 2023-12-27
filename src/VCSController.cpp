#include "../include/VCSController.h"
#include <fstream>
#include <iostream>

VCSController::VCSController() {
    // Constructor logic (if any)
}

void VCSController::init(const std::string& directory) {
    // Implementation of repository initialization
    currentDirectory = directory;
    std::cout << "Repository initialized at " << directory << std::endl;
}

void VCSController::add(const std::string& filename) {
    // Add file to staging area
    stagedFiles.push_back(filename);
    std::cout << filename << " added to staging area" << std::endl;
}

void VCSController::commit(const std::string& message) {
    // Commit staged changes
    if (stagedFiles.empty()) {
        std::cout << "No changes to commit" << std::endl;
        return;
    }
    
    saveCommit(message);
    stagedFiles.clear();
    std::cout << "Changes committed: " << message << std::endl;
}

void VCSController::rollback(const std::string& commitId) {
    // Rollback to a specific commit
    loadCommit(commitId);
    std::cout << "Rolled back to commit " << commitId << std::endl;
}

bool VCSController::isRepositoryInitialized() const {
    // Check if the repository is initialized
    // This could involve checking for a specific directory or file
    return !currentDirectory.empty();
}

void VCSController::saveCommit(const std::string& message) {
    // Save the commit to a file or database
    // This is a placeholder implementation
    std::ofstream file("commit_log.txt", std::ios::app);
    if (file.is_open()) {
        file << "Commit: " << message << "\n";
        for (const auto& filename : stagedFiles) {
            file << " - " << filename << "\n";
        }
        file.close();
    }
}

void VCSController::loadCommit(const std::string& commitId) {
    // Load a specific commit
    // This is a placeholder implementation
    // Actual implementation will depend on how you are storing the commits
}

