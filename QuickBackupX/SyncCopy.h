#pragma once
#include "pch.h"

class SyncCopy {

    struct File {
        fs::path src, dst;
    };

    int maxFiles;
    std::queue<File> list;

    void directories(fs::path src, std::string dst);

public:
    
    SyncCopy(std::string src, std::string dst, int mfpt = 0);

    void tick();
    bool done();

};