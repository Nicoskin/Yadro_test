#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Event.h"

std::vector<Event> readEvents(std::ifstream& infile);
void processEvents(const std::vector<Event>& events, int tableCount, const std::string& startTime, const std::string& endTime);

#endif // UTILS_H
