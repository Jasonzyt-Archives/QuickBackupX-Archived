#include "pch.h"
#include "AutoBackup.h"
#include "Utils.h"
#pragma warning(disable: 4996)

bool operator==(const Point& l, const Point& r) {
    return l.hour == r.hour && l.min == r.min;
}

bool checkTime(const std::string& str) {
    for (auto& c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

void AutoBackup::runBackup(bool remote) {
}

void AutoBackup::addPoint(const std::string& t, bool once, bool remote) {
    auto p = toPoint(t);
    p.once = once;
    p.remote = remote;
    points.push_back(p);
}

bool AutoBackup::cancel(Point p) {
    for (auto& po : points) {
        if (p == po) {
            p.ignore = true;
            return true;
        }
    }
    return false;
}

void AutoBackup::tick() {
    if (timer == 100) {
        timer = 0;
        auto ts = time(0);
        auto now = localtime(&ts);
        for (auto it = points.begin(); it != points.end(); it++) {
            auto& p = *it;
            if (p.hour == now->tm_hour && p.min == now->tm_min) {
                if (p.ignore) {
                    p.ignore = false;
                    break;
                }
                runBackup(p.remote);
                if (p.once) {
                    points.erase(it);
                }
                break;
            }
        }
        return;
    }
    timer++;
}

Point AutoBackup::toPoint(const std::string& str) {
    auto vec = split(str, ':');
    if (vec.size() != 2) {
        logger.warn(tr("Wrong AutoBackup time format: {}. Ignored.", str));
        return {};
    }
    if (!checkTime(vec[0]) || !checkTime(vec[1])) {
        logger.warn(tr("Wrong time: {}. Ignored.", str));
        return {};
    }
    auto h = stoi(vec[0]);
    auto m = stoi(vec[1]);
    if (h < 0 || h >= 24 || m < 0 || m >= 60) {
        logger.warn(tr("Wrong time: {}. Ignored.", str));
        return {};
    }
    return {h, m};
}