#ifndef VCS_H
#define VCS_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <sstream>

namespace fs = std::filesystem;
using namespace std::chrono;


class VCS {

public:
    VCS();

    void init(const std::string &path);
    void add(const std::string& filename);
    void commit(const std::string& message);
    void revert(const std::string& commitID);
    void log();

private:
    std::string basePath;
    fs::path findLastCommitFile(const std::string& filename);
    std::string getFileHash(const fs::path& path);
    std::vector<std::string> readFileLines(const std::string& filePath);
    void writeDifferences(const std::string& oldFilePath, const std::string& newFilePath, std::ofstream& diffFile);
    void writeNewFileContents(const std::string& filePath, std::ofstream& commitFile);
    bool shouldIgnore(const std::string& filename);
    void revertDirectory(const fs::directory_entry& source, const std::string& destinationPath);
    bool isIgnored(const std::string& filename) const;
    void copyToStaging(const fs::path& source, const std::string& destination);
    bool addInLog(std::string commitFolder, std::string author, std::string date,
                  const std::string& message, size_t filesChanged, size_t filesCreated);
    std::string hashFile(const fs::path& path);
    std::string hashDirectory(const fs::path& dirPath);

};

#endif // VCS_H
