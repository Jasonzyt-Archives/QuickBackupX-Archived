#pragma once
#include <string>
#include <ctime>

class ZipArchive {

    std::string path;
    void* stream = nullptr;
    void* zip = nullptr;

    static std::string readBinary(const std::string& path);

public:

    ZipArchive() = default;
    ZipArchive(const std::string& path);
    ~ZipArchive();
    void open(const std::string& path = "");
    void add(const std::string& path, const std::string& content, time_t mt = time(0));
    void addFile(const std::string& path, const std::string& file);
    void addFolder(const std::string& dir, const std::string& path = "");
    void close();
    void save(const std::string& path = "");
    void release();
    bool isOpen();

};