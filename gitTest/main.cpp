
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
    std::ifstream fileStream(path, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    std::ostringstream ss;
    ss << fileStream.rdbuf();
    std::string fileContent = ss.str();

    std::hash<std::string> hasher;
    size_t hashValue = hasher(fileContent);

    std::stringstream hashStream;
    hashStream << hashValue;
    return hashStream.str();
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
                    else{
                        std::cout << "should not copy";
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



std::vector<std::string> readFileLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

void writeDifferences(const std::string& oldFilePath, const std::string& newFilePath, std::ofstream& diffFile) {
    auto oldLines = readFileLines(oldFilePath);
    auto newLines = readFileLines(newFilePath);

    size_t oldSize = oldLines.size();
    size_t newSize = newLines.size();
    size_t maxLines = std::max(oldSize, newSize);

    for (size_t i = 0; i < maxLines; ++i) {
        if (i >= oldSize) {
            diffFile << "+ " << newLines[i] << "\n";
        } else if (i >= newSize) {
            diffFile << "- " << oldLines[i] << "\n";
        } else if (oldLines[i] != newLines[i]) {
            diffFile << "- " << oldLines[i] << "\n";
            diffFile << "+ " << newLines[i] << "\n";
        }
    }
}


void writeNewFileContents(const std::string& filePath, std::ofstream& commitFile) {
    auto lines = readFileLines(filePath);
    for (const auto& line : lines) {
        commitFile << "+ " << line << "\n";
    }
}

void commit(const std::string& message) {
    if (fs::is_empty(".git/staging")) {
        std::cerr << "Error: Staging area is empty. Nothing to commit.\n";
        return;
    }

    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::string commitFolder = ".git/commits/" + std::to_string(timestamp);

    std::vector<std::string> modifiedFiles;
    std::vector<std::string> newFiles;

    try {
        fs::create_directories(commitFolder);
        std::ofstream commitFile(commitFolder + "/commit_info.txt");

        for (const auto& entry : fs::directory_iterator(".git/staging")) {
            std::string filename = entry.path().filename().string();
            fs::path lastCommitFile = findLastCommitFile(filename);

            if (!lastCommitFile.empty()) {
                if (getFileHash(entry.path()) != getFileHash(lastCommitFile)) {
                    modifiedFiles.push_back(filename);
                    // Write differences for modified files
                    commitFile << "Modified: " << filename << "\n";
                    writeDifferences(lastCommitFile.string(), entry.path().string(), commitFile);
                }
            } else {
                newFiles.push_back(filename);
                commitFile << "Created: " << filename << "\n";
                writeNewFileContents(entry.path().string(), commitFile);
            }
            fs::copy(entry.path(), commitFolder + "/" + filename);
        }

        // Logic to find deleted files would go here
        // ...

        commitFile << "Author: <Your Name>\n";
        commitFile << "Date: " << std::ctime(&timestamp);
        commitFile << "Message: " << message << "\n";
        commitFile << "Files Changed: " << modifiedFiles.size() + newFiles.size()  << "\n";
        commitFile << "Files Created: " << newFiles.size() << "\n";

        fs::remove_all(".git/staging");
        fs::create_directory(".git/staging");
    } catch (const std::exception& e) {
        std::cerr << "Error during commit: " << e.what() << std::endl;
    }
}

// Add more filenames to ignore if needed   
bool shouldIgnore(const std::string& filename) {  
    return filename == "main.cpp" || filename == "main.exe" || filename == ".git" || filename == "build"; }

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
    vcs.add(".");
    vcs.commit("commit information");

    // Make changes to files...

    // vcs.add("a");
    //vcs.add("test.txt");

    // vcs.commit("Add file.txt");

    // Revert to the initial commit
    // vcs.revert(".git/commits/1703627053");

    return 0;
}