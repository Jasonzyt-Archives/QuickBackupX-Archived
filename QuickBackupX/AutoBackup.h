#pragma once
#include <vector>
#include <string>

struct Point {
    int hour = -1;
    int min = 0;
    bool once = false;
    bool ignore = false;
    bool remote = false;
};

bool operator==(const Point& l, const Point& r);

class AutoBackup {

    std::vector<Point> points;
    int timer = 0;

    void runBackup(bool remote);

public:

    AutoBackup() = default;

    void addPoint(const std::string& t, bool once = false, bool remote = false);
    bool cancel(Point p);
    void tick();

    static Point toPoint(const std::string& str);
};