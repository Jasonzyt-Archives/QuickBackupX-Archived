#pragma once
#include "pch.h"

class Executor {
    enum Type { Unknown, Console, Player, Block, AutoBackup } type;
    std::string pXuid;
};

class Backup {

public:

    std::string id;
    time_t time;
    Executor exer;

    virtual bool del() = 0;
    virtual bool resume() = 0;
    virtual bool isRemote() = 0;

};

class LocalBackup : public Backup {

public:

    std::string md5;
    std::string path;

    bool del();
    bool resume();
    bool isRemote();
    static LocalBackup* make(std::string path, Executor exer);

private:

    static bool checkDiskSpace(size_t size);
    static bool packZip(std::string path, std::string dst);

};
