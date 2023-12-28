#ifndef VCS_H
#define VCS_H

#include <string>
#include <vector>
#include <map>

class VCS {
public:
    VCS();

    void init();
    void add(const std::string& filename);
    void commit(const std::string& message);
    void revert(const std::string& commitID);
    void log() const;

private:
    fs::path findLastCommitFile(const std::string& filename);
    std::string getFileHash(const fs::path& path);
    bool addInLog(const std::string& commitFolder, const std::string& author, const std::string& date,
              const std::string& message, size_t filesChanged, size_t filesCreated); 
    std::vector<std::string> readFileLines(const std::string& filePath);
    void writeDifferences(const std::string& oldFilePath, const std::string& newFilePath, std::ofstream& diffFile);
    void writeNewFileContents(const std::string& filePath, std::ofstream& commitFile);
    bool shouldIgnore(const std::string& filename);
    void revertDirectory(const fs::directory_entry& source, const std::string& destinationPath);
    bool isIgnored(const std::string& filename) const;
    void copyToStaging(const fs::path& source, const std::string& destination);

};

#endif // VCS_H
