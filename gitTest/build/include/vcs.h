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
    void revert(std::string& commitID);
    void log() const;
};

#endif // VCS_H
