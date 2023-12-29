#include "vcs.h"
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


VCS::VCS() : basePath("") {
    // Default constructor, could initialize basePath or leave it for init method
}

// Default constructor
    void VCS::init(const std::string &path) {
        basePath = fs::absolute(path);
        std::cout << basePath<<std::endl;// Set the base path when initializing

        // Ensure the base path exists
        if (!fs::exists(basePath)) {
            fs::create_directories(basePath);
        }

        // Create .git directory and subdirectories in the specified basePath
        fs::create_directory(basePath +"/.git");
        fs::create_directory(basePath +"/.git/commits");
        fs::create_directory(basePath +"/.git/staging");

        // Create an empty log.json if it doesn't exist
        if (!fs::exists(basePath+"/log.json")) {
            std::ofstream file(basePath+"/log.json");
            file << "{}"; // Initializes the file with an empty JSON object
            file.close();
        }

        std::cout << "Repository initialized at " << basePath << std::endl;
    }


    void VCS::add(const std::string &relativePath) {

        auto path = basePath+ "/"+relativePath; // Use the base path

        const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;

        auto processEntry = [&](const fs::directory_entry &entry) {
            if (entry.is_regular_file() || entry.is_directory()) {
                std::string filename = entry.path().filename().string();
                std::cout << "file "<<filename << std::endl;
                if (!shouldIgnore(filename)) {
                    std::string destinationPath = basePath + "/.git/staging/" + filename;
                    fs::path lastCommitFile = findLastCommitFile(filename);

                    try {
                        bool shouldCopy = true;
                        if (!lastCommitFile.empty()) {

                            std::string lastCommitHash = getFileHash(lastCommitFile);
                            std::cout << lastCommitFile << std::endl;
                            std::cout << entry.path()<<std::endl;
                            std::string currentFileHash = getFileHash(entry.path());
                            shouldCopy = (lastCommitHash != currentFileHash);
                        }

                        if (shouldCopy) {
                            if (fs::exists(destinationPath)) {
                                fs::remove_all(destinationPath);
                            }
                            fs::copy(entry.path(), destinationPath, copyOptions);
                        } else {
                            std::cout << "should not copy";
                        }
                    } catch (const std::exception &e) {
                        std::cerr << "Error processing file or directory: " << e.what() << std::endl;
                    }
                }
            }
        };

        if (relativePath == ".") {
            // Add all files and directories in the current directory to the staging area
            for (const auto &entry: fs::directory_iterator(basePath)) {
                std::cout << entry.path()<<std::endl;
                processEntry(entry);
            }
        } else {
            // Add a specific file or directory to the staging area
            fs::path filePath = fs::path(path);
            processEntry(fs::directory_entry(filePath));
        }
    }


    bool VCS::addInLog(std::string commitFolder, std::string author, std::string date,
                       const std::string &message, size_t filesChanged, size_t filesCreated) {
        std::string logPath = basePath + "/log.json";
        std::ifstream inFile(logPath);
        std::string logContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        // Extracting only the last part of the commitFolder
        std::string commitId = commitFolder.substr(commitFolder.find_last_of('/') + 1);

        // Remove newline from the date if it exists
        std::string formattedDate = date;
        formattedDate.erase(std::remove(formattedDate.begin(), formattedDate.end(), '\n'), formattedDate.end());

        // Constructing the new log entry with newlines for pretty-printing
        std::string entry = "\"" + commitId + "\":{"
                                              "\"Author\":\"" + author + "\","
                                                                         "\"Date\":\"" + formattedDate + "\","
                                                                                                         "\"Message\":\"" +
                            message + "\","
                                      "\"Files Changed\":" + std::to_string(filesChanged) + ","
                                                                                            "\"Files Created\":" +
                            std::to_string(filesCreated) + "}";

        // Determine whether we need to prepend a comma
        std::string prefix = (logContent.empty() || logContent == "{}") ? "" : ",\n";

        // Open file in truncate mode to overwrite
        std::ofstream outFile(logPath, std::ios::trunc);
        if (!outFile.is_open()) {
            std::cerr << "Error: Unable to open log.json for writing.\n";
            return false;
        }

        // If the log was empty or contained only {}, we start a new JSON object
        if (logContent.empty() || logContent == "{}") {
            outFile << "{\n" << entry << "\n}";
        } else {
            // Otherwise, we insert the new entry before the closing brace
            size_t lastBracePos = logContent.find_last_of('}') - 1;
            logContent.insert(lastBracePos, prefix + entry);
            outFile << logContent;
        }

        outFile.close();
        return true;
    }


    void VCS::commit(const std::string &message) {
        if (fs::is_empty(basePath+"/.git/staging")) {
            std::cerr << "Error: Staging area is empty. Nothing to commit.\n";
            return;
        }

        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        std::string commitFolder = basePath + "/.git/commits/" + std::to_string(timestamp);

        std::vector <std::string> modifiedFiles;
        std::vector <std::string> newFiles;

        try {
            fs::create_directories(commitFolder);
            std::ofstream commitFile(commitFolder + "/commit_info.txt");

            for (const auto &entry: fs::directory_iterator(basePath + "/.git/staging")) {
                std::string filename = entry.path().filename().string();
                fs::path lastCommitFile = findLastCommitFile(filename);

                if (!lastCommitFile.empty()) {
                    std::cout << "dkhlna hna"<<std::endl;
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
                std::cout << commitFolder << std::endl;
                std::cout << entry.path()<<std::endl;
                std::cout << filename << std::endl;
                fs::copy(entry.path(), commitFolder + "/" + filename);
            }

            // Logic to find deleted files would go here
            // ...

            commitFile << "Author: <Your Name>\n";
            commitFile << "Date: " << std::ctime(reinterpret_cast<const time_t *>(&timestamp));
            commitFile << "Message: " << message << "\n";
            commitFile << "Files Changed: " << modifiedFiles.size() + newFiles.size() << "\n";
            commitFile << "Files Created: " << newFiles.size() << "\n";

            fs::remove_all(basePath+"/.git/staging");
            fs::create_directory(basePath+"/.git/staging");
        } catch (const std::exception &e) {
            std::cerr << "Error during commit: " << e.what() << std::endl;
        }

        std::string authorName = "<Your Name>"; // Replace with actual author name or variable
        std::string dateString = std::ctime(
                reinterpret_cast<const time_t *>(&timestamp)); // Convert timestamp to readable date

        // After a successful commit:
        if (addInLog(commitFolder, authorName, dateString, message, modifiedFiles.size(), newFiles.size())) {
            std::cout << "Commit logged successfully.\n";
        } else {
            std::cerr << "Failed to log commit.\n";
        }
    }


    void VCS::revert(const std::string &commitId) {
        std::string logPath = basePath + "/log.json";
        std::ifstream inFile(logPath);
        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open log.json.\n";
            return;
        }
        std::string logContent((std::istreambuf_iterator<char>(inFile)),
                               std::istreambuf_iterator<char>());
        inFile.close();

        // Check if commitId exists in the log
        if (logContent.find("\"" + commitId + "\":{") == std::string::npos) {
            std::cerr << "Error: The commit id " << commitId << " does not exist.\n";
            return;
        }

        // Construct the path to the commit folder
        std::string commitFolderPath = basePath + "/.git/commits/" + commitId;
        if (!fs::exists(commitFolderPath) || !fs::is_directory(commitFolderPath)) {
            std::cerr << "Error: The commit folder for " << commitId << " does not exist.\n";
            return;
        }

        // Iterate through the files in the commit folder
        for (const auto &file: fs::directory_iterator(commitFolderPath)) {
            fs::path filePath = file.path();
            std::string filename = filePath.filename().string();

            if (!shouldIgnore(filename)) {
                fs::path targetPath = filename; // Assumes the main folder is the current working directory

                // Remove the file if it already exists in the main folder
                if (fs::exists(targetPath)) {
                    fs::remove(targetPath);
                }

                // Copy file from commit folder to the main folder
                try {
                    fs::copy(filePath, targetPath);
                } catch (const fs::filesystem_error &e) {
                    std::cerr << "Error copying file " << filePath << ": " << e.what() << '\n';
                }
            }
        }

        std::cout << "Reverted to commit " << commitId << ".\n";
    }


fs::path VCS::findLastCommitFile(const std::string &filename) {
    std::vector<fs::path> commitFolders;

    // Gather all commit folders
    for (const auto &entry : fs::directory_iterator(basePath + "/.git/commits")) {
        if (entry.is_directory()) {
            commitFolders.push_back(entry.path());
        }
    }

    // Sort commit folders by timestamp in descending order
    std::sort(commitFolders.rbegin(), commitFolders.rend());

    // Iterate through commit folders to find the most recent file
    for (const auto &folder : commitFolders) {
        std::cout << folder << std::endl;
        fs::path potentialFile = folder / filename;
        if (fs::exists(potentialFile)) {
            std::cout << "Found file: " << potentialFile << std::endl;
            return potentialFile;
        }
    }

    return fs::path(); // Return an empty path if not found
}


    std::string VCS::getFileHash(const fs::path &path) {
        std::ifstream fileStream(path, std::ios::binary);
        if (!fileStream) {
            throw std::runtime_error("Cannot open file: " + path.string());
        }

        std::ostringstream ss;
        ss << fileStream.rdbuf();
        std::string fileContent = ss.str();

        std::hash <std::string> hasher;
        size_t hashValue = hasher(fileContent);

        std::stringstream hashStream;
        hashStream << hashValue;
        return hashStream.str();
    }


    std::vector <std::string> VCS::readFileLines(const std::string &filePath) {
        std::vector <std::string> lines;
        std::ifstream file(filePath);
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        return lines;
    }


    void VCS::writeDifferences(const std::string &oldFilePath, const std::string &newFilePath, std::ofstream &diffFile) {
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
    void VCS::writeNewFileContents(const std::string &filePath, std::ofstream &commitFile) {
        auto lines = readFileLines(filePath);
        for (const auto &line: lines) {
            commitFile << "+ " << line << "\n";
        }
    }
    bool VCS::shouldIgnore(const std::string &filename) {
        return filename == "main.cpp" || filename == "main.exe" || filename == ".git" || filename == "build" ||
               filename == "log.json" || filename == "commit_info.txt";
    }

    void VCS::revertDirectory(const fs::directory_entry &source, const std::string &destinationPath) {
        if (fs::exists(destinationPath)) {
            fs::remove_all(destinationPath);  // Remove existing directory
        }
        fs::copy(source.path(), destinationPath, fs::copy_options::recursive);
    }

    bool VCS::isIgnored(const std::string &filename) const {
        return filename == "main.cpp" || filename == "main.exe" || filename == ".git" || filename == "build" ||
               filename == "log.json" || filename == "commit_info.txt";
    }
    void VCS::copyToStaging(const fs::path &source, const std::string &destination) {
        std::string destinationPath = basePath+"/"+destination + source.filename().string();
        if (fs::exists(destinationPath)) {
            try { fs::remove_all(destinationPath); } catch (const std::exception &e) {
                std::cerr << "Error removing existing file or directory: " << e.what() << std::endl;
            }
        }
        fs::copy(source, destination + source.filename().string(), fs::copy_options::recursive);
    };


    void VCS::log() {
        std::string logPath = basePath + "/log.json";
        std::ifstream inFile(logPath);

        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open log.json for reading." << std::endl;
            return;
        }

        std::string logContent((std::istreambuf_iterator<char>(inFile)),
                               std::istreambuf_iterator<char>());
        inFile.close();

        // Split the content at '},' to get individual commit entries
        std::istringstream iss(logContent);
        std::string entry;
        while (std::getline(iss, entry, '}')) {
            // Skip empty lines and the final closing brace
            if (entry.empty() || entry.find('{') == std::string::npos) {
                continue;
            }

            // Add back the removed closing brace for parsing
            entry += '}';

            // Trim leading/trailing whitespaces and other formatting
            entry.erase(std::remove(entry.begin(), entry.end(), '\n'), entry.end());
            entry.erase(std::remove(entry.begin(), entry.end(), '\t'), entry.end());

            // Find the commit ID and print it
            size_t commitStart = entry.find_first_of("\"") + 1;
            size_t commitEnd = entry.find("\":{");
            if (commitStart != std::string::npos && commitEnd != std::string::npos) {
                std::cout << "Commit ID: " << entry.substr(commitStart, commitEnd - commitStart) << std::endl;
            }

            // Extract and print each attribute
            size_t startPos = entry.find_first_of('{') + 1;
            size_t endPos = entry.find_last_of('}');
            if (startPos != std::string::npos && endPos != std::string::npos) {
                std::string attributes = entry.substr(startPos, endPos - startPos);

                // Split the attributes at ',' to get key-value pairs
                std::istringstream attrStream(attributes);
                std::string attribute;
                while (std::getline(attrStream, attribute, ',')) {
                    size_t colonPos = attribute.find(':');
                    if (colonPos != std::string::npos) {
                        std::string key = attribute.substr(0, colonPos);
                        key.erase(std::remove(key.begin(), key.end(), '\"'), key.end()); // Remove quotes

                        std::string value = attribute.substr(colonPos + 1);
                        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end()); // Remove quotes

                        // Print the key-value pair
                        std::cout << key << ": " << value << std::endl;
                    }
                }
            }

            // Print two newlines between entries
            std::cout << std::endl << std::endl;
        }
    }
