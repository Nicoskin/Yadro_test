#ifndef EVENT_H
#define EVENT_H

#include <string>

struct Event {
    std::string time;
    int id;
    std::string client;
    int table = 0;
};

bool isValidTime(const std::string& time);
int roundUpHours(const std::string& start, const std::string& end);

#endif // EVENT_H
