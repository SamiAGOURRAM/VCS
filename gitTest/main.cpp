
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm>
#include <filesystem>
#include <vector>


namespace fs = std::filesystem;
using namespace std::chrono;

fs::path findLastCommitFile(const std::string& filename) {

    std::vector<fs::path> commitFolders;
    
    // Gather all commit folders
    for (const auto& entry : fs::directory_iterator(".git/commits")) {
        if (entry.is_directory()) {
            commitFolders.push_back(entry.path());
        }
    }

    // Sort commit folders by timestamp in descending order
    std::sort(commitFolders.rbegin(), commitFolders.rend());

    // Iterate through commit folders to find the most recent file
    for (const auto& folder : commitFolders) {
        fs::path potentialFile = folder / filename;
        if (fs::exists(potentialFile)) {
            return potentialFile;
        }
    }

    return fs::path(); // Return an empty path if not found
}

class VersionControl {
public:
    void init() {
        // Create .git directory and subdirectories
        fs::create_directory(".git");
        fs::create_directory(".git/commits");
        fs::create_directory(".git/staging");
    }


std::string getFileHash(const fs::path& path) {
    // Implement a function to calculate the hash of a file
    // For example, you could use SHA-1, MD5, or any other hash function
    // ...
}


void add(const std::string& path) {
    const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;

    auto processEntry = [&](const fs::directory_entry& entry) {
        if (entry.is_regular_file() || entry.is_directory()) {
            std::string filename = entry.path().filename().string();
            if (!shouldIgnore(filename)) {
                std::string destinationPath = ".git/staging/" + filename;
                fs::path lastCommitFile = findLastCommitFile(filename);

                try {
                    bool shouldCopy = true;
                    if (!lastCommitFile.empty()) {
                        std::string lastCommitHash = getFileHash(lastCommitFile);
                        std::string currentFileHash = getFileHash(entry.path());
                        shouldCopy = (lastCommitHash != currentFileHash);
                    }

                    if (shouldCopy) {
                        if (fs::exists(destinationPath)) {
                            fs::remove_all(destinationPath);
                        }
                        fs::copy(entry.path(), destinationPath, copyOptions);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error processing file or directory: " << e.what() << std::endl;
                }
            }
        }
    };

    if (path == ".") {
        // Add all files and directories in the current directory to the staging area
        for (const auto& entry : fs::directory_iterator(".")) {
            processEntry(entry);
        }
    } else {
        // Add a specific file or directory to the staging area
        fs::path filePath = fs::path(path);
        processEntry(fs::directory_entry(filePath));
    }
}


void commit(const std::string& message) {
    if (fs::is_empty(".git/staging")) {
        std::cerr << "Error: Staging area is empty. Nothing to commit.\n";
        return;
    }

    auto timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    std::string commitFolder = ".git/commits/" + std::to_string(timestamp);

    try {
        // Ensure the commit folder exists
        fs::create_directories(commitFolder);

        // Move files from staging area to the commit folder
        for (const auto& entry : fs::directory_iterator(".git/staging")) {
            if (!fs::exists(entry.path())) {
                std::cerr << "Warning: File not found, skipping: " << entry.path() << std::endl;
                continue;
            }
            std::string destination = commitFolder + "/" + entry.path().filename().string();
            fs::copy(entry.path(), destination);
        }

        // Write commit information to a file
        std::ofstream commitFile(commitFolder + "/commit_info.txt");
        commitFile << "Author: <Your Name>\n";
        commitFile << "Date: " << ctime(&timestamp);
        commitFile << "Message: " << message << "\n";

        // Clear the staging area
        fs::remove_all(".git/staging");
        fs::create_directory(".git/staging");
    } catch (const std::exception& e) {
        std::cerr << "Error during commit: " << e.what() << std::endl;
    }
}




void revert(const std::string& commitFolder) {
    // Revert to a specific commit by copying files and directories from the commit folder to the current directory
    for (const auto& entry : fs::directory_iterator(commitFolder)) {
        if (entry.is_regular_file() || entry.is_directory()) {
            if (entry.path().filename() == "commit_info.txt") {
                // Skip copying commit_info.txt
                continue;
            }

            std::string destinationPath = (fs::current_path() / entry.path().filename()).string();
            try {
                if (entry.is_directory()) {
                    revertDirectory(entry, destinationPath);
                } else {
                    revertFile(entry, destinationPath);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error reverting file or directory: " << e.what() << std::endl;
            }
        }
    }
}

// Add more filenames to ignore if needed   
bool shouldIgnore(const std::string& filename) {  return filename == "main.cpp" || filename == "main.exe" || filename == ".git"; }

void revertDirectory(const fs::directory_entry& source, const std::string& destinationPath) {
    if (fs::exists(destinationPath)) {
        fs::remove_all(destinationPath);  // Remove existing directory
    }
    fs::copy(source.path(), destinationPath, fs::copy_options::recursive);
}

void revertFile(const fs::directory_entry& source, const std::string& destinationPath) {
    if (fs::exists(destinationPath)) {
        fs::remove(destinationPath);  // Remove existing file
    }
    fs::copy(source.path(), destinationPath);
}



    /*void copyToStaging(const fs::path& source, const std::string& destination) {
        std::string destinationPath = destination + source.filename().string();
        try {
            fs::remove_all(destinationPath);
        } catch (const std::exception& e) {
            std::cerr << "Error removing existing file or directory: " << e.what() << std::endl;
            return;
        }

        try {
            fs::copy(source, destinationPath, fs::copy_options::recursive);
        } catch (const std::exception& e) {
            std::cerr << "Error copying file or directory to staging: " << e.what() << std::endl;
        }
    }

};*/

private:     bool isIgnored(const std::string& filename) const {       return filename == "main.cpp" || filename == "main.exe" || filename == ".git";     }     
void copyToStaging(const fs::path& source, const std::string& destination) {         std::string destinationPath = destination + source.filename().string();         if (fs::exists(destinationPath)) {             try {                 fs::remove_all(destinationPath);             } catch (const std::exception& e) {                 std::cerr << "Error removing existing file or directory: " << e.what() << std::endl;             }         }         fs::copy(source, destination + source.filename().string(), fs::copy_options::recursive);     } };

int main() {
    VersionControl vcs;

    // Example usage
    //vcs.init();

// 
    //vcs.add(".");
    //vcs.commit("Initial commit");

    std::cout << findLastCommitFile("test.txt");

    // Make changes to files...

    // vcs.add("a");
    //vcs.add("test.txt");

    // vcs.commit("Add file.txt");

    // Revert to the initial commit
    // vcs.revert(".git/commits/1703627053");

    return 0;
}