#pragma once
#include <vector>
#include <string>

struct Point {
    int hour = 0;
    int min = 0;
    bool once = false;
    bool ignore = false;
};

class AutoBackup {

    std::vector<Point> points;
    int timer = 0;

    void runBackup();

public:

    AutoBackup() = default;

    void addPoint(const std::string& t, bool once = false);
    void cancel(Point p);
    void tick();

};