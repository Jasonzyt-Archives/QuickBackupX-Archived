#include "ZipArchive.h"
#include <minizip/mz.h>
#include <minizip/mz_zip.h>
#include <minizip/mz_strm.h>
#include <minizip/mz_strm_os.h>
#include <minizip/mz_strm_mem.h>
#include <minizip/mz_zip_rw.h>
#include <fstream>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

template <typename T>
time_t chrono2time_t(T tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - T::clock::now()
        + system_clock::now());
    return system_clock::to_time_t(sctp);
}

ZipArchive::ZipArchive(const std::string& path) {
    open(path);
}
ZipArchive::~ZipArchive() {
    release();
}

void ZipArchive::open(const std::string& path) {
    if (!path.empty()) {
        if (!fs::exists(path)) {
            fs::create_directories(fs::path(path).parent_path());
        }
        this->path = path;
    }
    mz_stream_mem_create(&stream);
    mz_stream_mem_set_grow_size(stream, (128 * 1024));
    mz_stream_open(stream, path.empty() ? NULL : path.c_str(), MZ_OPEN_MODE_CREATE);
    mz_zip_create(&zip);
    auto err = mz_zip_open(zip, stream, MZ_OPEN_MODE_WRITE);
    if (err != MZ_OK) {
        auto msg = "Error in mz_zip_open: " + std::to_string(err);
        throw std::exception(msg.c_str());
    }
}

void ZipArchive::add(const std::string& path, const std::string& content, time_t mt) {
    mz_zip_file file{};
    mz_zip_get_version_madeby(zip, &file.version_madeby);
    file.compression_method = MZ_COMPRESS_LEVEL_FAST;
    file.filename = path.c_str();
    file.filename_size = path.size();
    file.creation_date = file.accessed_date = time(0);
    file.modified_date = mt;
    file.flag = MZ_ZIP_FLAG_UTF8;
    mz_zip_entry_write_open(zip, &file, MZ_COMPRESS_LEVEL_FAST, 1, NULL);
    mz_zip_entry_write(zip, content.c_str(), content.size());
    mz_zip_entry_close(zip);
}
void ZipArchive::addFile(const std::string& path, const std::string& file) {
    auto content = readBinary(file);
    auto ftm = fs::last_write_time(path);
    auto tm = chrono2time_t(ftm);
    add(path, content, tm);
}
void ZipArchive::addFolder(const std::string& dir, const std::string& path) {
    if (fs::is_directory(dir)) {
        for (auto& p : fs::directory_iterator(dir)) {
            if (p.is_directory()) {
                addFolder(p.path().string(), path + p.path().filename().string() + "/");
            } else {
                addFile(path + p.path().filename().string(), p.path().string());
            }
        }
    }
}

void ZipArchive::close() {
    mz_zip_close(zip);
}

void ZipArchive::save(const std::string& path) {
    std::string p = path;
    if (p.empty()) p = this->path;
    if (p.empty()) {
        throw std::exception("No zip file path specified");
    }
    void* osStream = nullptr;
    std::byte* buf = nullptr;
    mz_stream_mem_get_buffer(stream, (const void**)&buf);
    mz_stream_mem_seek(stream, 0, MZ_SEEK_END);
    size_t size = mz_stream_mem_tell(stream);
    mz_stream_os_create(&osStream);
    mz_stream_os_open(osStream, p.c_str(), MZ_OPEN_MODE_WRITE | MZ_OPEN_MODE_CREATE);
    mz_stream_os_write(osStream, buf, size);
    mz_stream_os_close(osStream);
    mz_stream_os_delete(&osStream);
}

void ZipArchive::release() {
    mz_zip_delete(&zip);
    mz_stream_mem_delete(&stream);
}

bool ZipArchive::isOpen() {
    if (zip && stream) {
        return mz_zip_writer_is_open(zip) == MZ_OK && 
            mz_stream_is_open(stream) == MZ_OK;
    }
    return false;
}

std::string ZipArchive::readBinary(const std::string& path) {
    std::fstream file(path, std::ios::in | std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}