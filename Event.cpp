#include "Event.h"
#include <cstdlib> // Для std::stoi

// Функция для проверки корректности формата времени
bool isValidTime(const std::string& time) {
    if (time.length() != 5 || time[2] != ':') return false;
    int hours = std::stoi(time.substr(0, 2));
    int minutes = std::stoi(time.substr(3, 2));
    return (hours >= 0 && hours < 24) && (minutes >= 0 && minutes < 60);
}

// Функция для округления времени в большую сторону до целого часа
int roundUpHours(const std::string& start, const std::string& end) {
    int startHours = std::stoi(start.substr(0, 2));
    int startMinutes = std::stoi(start.substr(3, 2));
    int endHours = std::stoi(end.substr(0, 2));
    int endMinutes = std::stoi(end.substr(3, 2));

    if (endMinutes > 0) endHours++;
    return endHours - startHours;
}
