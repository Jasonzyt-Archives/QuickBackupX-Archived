#include "pch.h"
#include "AutoBackup.h"
#include "Utils.h"

bool checkTime(const std::string& str) {
    for (auto& c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

void AutoBackup::runBackup() {
}

void AutoBackup::addPoint(const std::string& t, bool once) {
    auto vec = split(t, ':');
    if (vec.size() != 2) {
        logger.warn(tr("Wrong AutoBackup time format: {}. Ignored.", t));
        return;
    }
    if (!checkTime(vec[0]) || !checkTime(vec[1])) {
        logger.warn(tr("Wrong time: {}. Ignored.", t));
        return;
    }
    auto h = stoi(vec[0]);
    auto m = stoi(vec[1]);
    if (h < 0 || h >= 24 || m < 0 || m >= 60) {
        logger.warn(tr("Wrong time: {}. Ignored.", t));
        return;
    }
    points.push_back({ h, m, once });
}

void AutoBackup::cancel(Point p) {
    for (auto& po : points) {
        if (p.hour == po.hour && p.min == po.min) {
            p.ignore = true;
        }
    }
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
                runBackup();
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