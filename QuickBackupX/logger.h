#ifndef LOGGER_H
#define LOGGER_H
#include "pch.h"

#if defined(BDS_V1_16)
    void registerASYNC2(struct asyncFStream*);
    void unregisterASYNC2(struct asyncFStream*, struct asyncFStream*);
#elif defined(BDS_V1_17)
    LIAPI void registerASYNC2(struct asyncFStream*);
    LIAPI void unregisterASYNC2(struct asyncFStream*, struct asyncFStream*);
#endif

struct stdio_commit {
    std::string_view prefix;
    void operator()(std::string_view extra, std::string_view content) const {
        std::cout << extra << prefix << content << '\n';
    }
    stdio_commit(std::string_view prefix_) : prefix(prefix_) {}
};
using std::string;
struct asyncFStream {
    std::ofstream ofs;
    std::unique_ptr<string> buf1;
    std::unique_ptr<string> buf2;
    std::atomic_flag _lock = ATOMIC_FLAG_INIT;
#define lock()                                            \
    while (_lock.test_and_set(std::memory_order_acquire)) \
    std::this_thread::yield()
#define unlock() _lock.clear()
    asyncFStream() {
        registerASYNC2(this);
        buf1 = std::make_unique<string>();
        buf2 = std::make_unique<string>();
        buf1->reserve(2048);
        buf2->reserve(2048);
    }
    asyncFStream(asyncFStream&& r) noexcept {
        unregisterASYNC2(&r, this);
        ofs = std::move(r.ofs);
        buf1 = std::move(r.buf1);
        buf2 = std::move(r.buf2);
    }
    ~asyncFStream() { unregisterASYNC2(this, nullptr); }
    inline void _flush_buffer(string* x) {
        ofs.write(x->data(), x->size());
        x->clear();
    }
    void close() {
        lock();
        _flush_buffer(buf1.get());
        ofs.close();
        unlock();
    }
    void open(const char* name) {
        lock();
        ofs.open(name, std::ios::app);
        ofs.rdbuf()->pubsetbuf(nullptr, 0);
        unlock();
    }
    void flushTimer() {
        if (buf1->size()) {
            lock();
            buf1.swap(buf2);
            unlock();
            _flush_buffer(buf2.get());
        }
    }
    template <typename... T>
    inline void write(T &&... x) {
        lock();
        (buf1->append(std::forward<T>(x)), ...);
        unlock();
    }
    template <typename... T>
    inline void writeLine(T &&... x) {
        lock();
        (buf1->append(std::forward<T>(x)), ...);
        buf1->push_back('\n');
        unlock();
    }
#undef lock
#undef unlock
};
using std::string;
struct file_commit {
    asyncFStream dat;
    const char* fn;
    unsigned int totalWrite = 0;
    unsigned int maxWrite;
    unsigned int maxLogs;
    string LogName(int i) {
        if (i == 0)
            return fn;
        return string(fn) + "." + std::to_string(i);
    }
    void CleanUpLogs() {
        using namespace std;
        try {
            filesystem::remove(LogName(maxLogs));
        }
        catch (...) {
        }
        for (int i = maxLogs - 1; i >= 0; --i) {
            try {
                filesystem::rename(LogName(i), LogName(i + 1));
            }
            catch (...) {
            }
        }
    }
    void TryTidyUp() {
        using namespace std;
        try {
            auto nowsz = filesystem::file_size(fn);
            if (nowsz >= maxWrite) {
                CleanUpLogs();
            }
        }
        catch (...) {
        }
    }
    file_commit(const char* fn_,
        unsigned int maxLogs_ = 3,
        unsigned int maxWrite_ = 4 * 1024 * 1024) {
        // maxWrite 4MB fn.log fn.log.1 fn.log.2
        fn = fn_;
        dat.open(fn);
        maxWrite = maxWrite_;
        maxLogs = maxLogs_;
        TryTidyUp();
    }
    file_commit(file_commit&& r) noexcept : dat(std::move(r.dat)) {
        maxWrite = r.maxWrite;
        maxLogs = r.maxLogs;
        fn = r.fn;
        totalWrite = 0;
    }
    file_commit(const file_commit&) = delete;
    void operator()(std::string_view extra, std::string_view content) {
        dat.writeLine(extra, content);
        totalWrite += (unsigned int)(extra.size() + content.size());
        if (totalWrite > maxWrite) {
            dat.close();
            TryTidyUp();
            dat.open(fn);
            totalWrite = 0;
        }
    }
};
template <typename... TP>
struct stacked {
    std::tuple<TP...> data;
    stacked(TP &&... args) noexcept : data(std::forward<TP>(args)...) {}
    template <size_t idx, typename... TC>
    void _call(TC &&... args) noexcept {
        std::get<idx>(data)(std::forward<TC>(args)...);
        if constexpr (idx != std::tuple_size_v<decltype(data)> -1) {
            _call<idx + 1, TC...>(std::forward<TC>(args)...);
        }
    }
    template <typename... TC>
    void operator()(TC &&... args) noexcept {
        _call<0, TC...>(std::forward<TC>(args)...);
    }
};

enum LOGLVL : char {
    INFO = 0,
    WARN = 1,
    ERROR = 2,
    FATAL = 3,
    DEBUG = 4
};
enum COLOR : unsigned int {
    BLUE = 9, // Sky blue
    GREEN = 10, 
    RED = 12,
    CYAN = 11,
    PINK = 13,
    YELLOW = 14,
    WHITE = 15,

    DK_BLUE = 1,
    DK_GREEN = 2,
    DK_RED = 4, 
    DK_CYAN = 3, // Bright blue
    DK_PINK = 5, // Purple
    DK_YELLOW = 6, 
    DK_WHITE = 7 // Grey
};

template <typename COMMITER>
class Logger {
    COMMITER cmt;
    LOGLVL lvlmin;
    std::string time_fmt;
    bool color_print;
    static constexpr std::string_view LVLNAME[5] = { "INFO]", "WARN]", "ERROR]", "FATAL]","DEBUG]" };
public:
    Logger(COMMITER&& cm) : cmt(std::forward<COMMITER>(cm))
    {
        time_fmt = "%Y-%m-%d %H:%M:%S";
        color_print = false;
        lvlmin = INFO;
    }
    Logger(COMMITER&& cm, bool color) : cmt(std::forward<COMMITER>(cm)), color_print(color)
    {
        time_fmt = "%Y-%m-%d %H:%M:%S";
        lvlmin = INFO;
    }
    Logger(COMMITER&& cm, bool color, std::string tm_fmt) : 
        cmt(std::forward<COMMITER>(cm)), 
        color_print(color), 
        time_fmt(tm_fmt)
    {
        lvlmin = INFO;
    }
    template <typename A, typename B>
    static inline void __helper(A& x, B&& y)
    {
        x << std::forward<B>(y);
    }
    template <typename A, typename B>
    static inline void __helper(A& x, B&& y, char z)
    {
        x << std::forward<B>(y) << z;
    }
    template <LOGLVL lvl, COLOR color = WHITE, char delim = 0, typename... TP>
    inline void _logimpl(TP&&... args)
    {
        char datebuf[512];
        struct tm _tim;
        {
            auto timnow = time(0);
            localtime_s(&_tim, &timnow);
        }
        auto timsz = strftime(datebuf, 256, ("[" + time_fmt + " ").c_str(), &_tim);
        strcat_s(datebuf, LVLNAME[int(lvl)].data());
        std::stringstream ss;
        if constexpr (delim != 0) {
            (__helper(ss, std::forward<TP>(args), delim), ...);
        }
        else {
            (__helper(ss, std::forward<TP>(args)), ...);
        }
        if (color_print) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        cmt->operator()(std::string_view{ datebuf, timsz + LVLNAME[int(lvl)].size() }, ss.str());
        if (color_print) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }
    template <LOGLVL lvl = INFO, COLOR color = WHITE, typename... TP>
    inline void operator()(TP&&... args) {
        p<lvl, color>(std::forward<TP>(args)...);
    }
    template <LOGLVL lvl = INFO, COLOR color = WHITE, char join = 0, typename... TP>
    inline void p(TP&&... args) {
        if (lvl >= lvlmin) {
            _logimpl<lvl, color, join>(std::forward<TP>(args)...);
        }
    }
};

#endif