#include "vcs.h"
#include <iostream>
#include <fstream>

VCS::VCS() {}

void VCS::init() {
    //TODO 
    std::cout << "Repository initialized." << std::endl;
}

void VCS::add(const std::string& filename) {
    //TODO
    std::cout << "Added " << filename << " to staging area." << std::endl;
}

void VCS::commit(const std::string& message) {
    //TODO
}

void VCS::revert(std::string& commitID) {

    //TODO

    std::cout << "Reverted to commit " << commitID << "." << std::endl;
}

void VCS::log() const {
  
}