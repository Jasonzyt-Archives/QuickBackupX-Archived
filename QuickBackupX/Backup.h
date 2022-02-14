#pragma once
#include "pch.h"

extern std::unordered_map<std::string, Backup> backups;

class Executor {
    enum Type { Unknown, Console, Player, Block, AutoBackup } type;
    std::string pName;
    std::string pXuid;
};

class Backup {

public:

    std::string id;
    time_t time = 0;
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
    static LocalBackup* make(const std::string& path, Executor exer);

private:

    static bool checkDiskSpace(char disk, size_t size);
    static bool packZip(const std::string& path, const std::string& dst);

};

class RemoteBackup : public Backup {

public:

    size_t size;

    bool del();
    bool resume();
    bool download();
    bool downloadZip();
    bool isRemote();
    static RemoteBackup* make(Executor exer);

private:

    bool uploadFiles();

};
