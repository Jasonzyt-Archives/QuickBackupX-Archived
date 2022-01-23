#pragma once
#include <string_view>
#include <iostream>
#include <sstream>
#include <ctime>
#include <Windows.h>

enum LOGLVL : char {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4,
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

#if defined(BDS_LL)

#include <Global.h>

template <typename Stream, typename T>
inline void __helper(Stream& x, T&& y) {
    x << std::forward<T>(y);
}
template <typename Stream, typename ... Args>
inline void appendStream(Stream& stream, Args&&... args) {
    (__helper(stream, std::forward<Args>(args)), ...);
}

template <LOGLVL lvl = INFO, COLOR color = WHITE, typename ... Args>
inline void lllog(Args... args) {
#if defined(BUILD_RELEASE)
    static int minLvl = (int)LOGLVL::INFO;
#else
    static int minLvl = (int)LOGLVL::DEBUG;
#endif
    if ((int)lvl >= minLvl) {
        std::stringstream ss;
        appendStream(ss, std::forward<Args>(args)...);
        std::string s = ss.str();
        switch (lvl) {
        case DEBUG:
            logger.debug("{}", s);
            break;
        case INFO:
            logger.info("{}", s);
            break;
        case WARN:
            logger.warn("{}", s);
            break;
        case ERROR:
            logger.error("{}", s);
            break;
        case FATAL:
            logger.fatal("{}", s);
            break;
        }
    }
}

#else 

struct stdio_commit {
    std::string_view prefix;
    void operator()(std::string_view extra, std::string_view content) const {
        std::cout << extra << prefix << content << '\n';
    }
    stdio_commit(std::string_view prefix_) : prefix(prefix_) {}
};

template <typename COMMITER>
class OLogger {
    COMMITER cmt;
#if defined(BUILD_RELEASE)
    LOGLVL lvlmin = INFO;
#else
    LOGLVL lvlmin = DEBUG;
#endif
    std::string time_fmt;
    bool color_print;
    static constexpr std::string_view LVLNAME[5] = { "DEBUG]", "INFO]", "WARN]", "ERROR]", "FATAL]" };
public:
    OLogger(COMMITER&& cm) : cmt(std::forward<COMMITER>(cm)) {
        time_fmt = "%Y-%m-%d %H:%M:%S";
        color_print = false;
    }
    OLogger(COMMITER&& cm, bool color) : cmt(std::forward<COMMITER>(cm)), color_print(color) {
        time_fmt = "%Y-%m-%d %H:%M:%S";
    }
    OLogger(COMMITER&& cm, bool color, std::string tm_fmt) : cmt(std::forward<COMMITER>(cm)), color_print(color),
        time_fmt(tm_fmt) {}
    template <typename A, typename B>
    static inline void __helper(A& x, B&& y) {
        x << std::forward<B>(y);
    }
    template <typename A, typename B>
    static inline void __helper(A& x, B&& y, char z) {
        x << std::forward<B>(y) << z;
    }
    template <LOGLVL lvl, COLOR color = WHITE, char delim = 0, typename... TP>
    inline void _logimpl(TP&&... args) {
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