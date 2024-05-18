#include <iostream>
#include <fstream>
#include "Event.h"
#include "Utils.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    int tableCount;
    infile >> tableCount;

    std::string startTime, endTime;
    infile >> startTime >> endTime;

    int eventCount;
    infile >> eventCount;

    if (!isValidTime(startTime) || !isValidTime(endTime)) {
        std::cerr << "Invalid time format." << std::endl;
        return 1;
    }

    std::vector<Event> events = readEvents(infile);

    processEvents(events, tableCount, startTime, endTime);

    return 0;
}
