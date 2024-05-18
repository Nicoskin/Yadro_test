#include "Utils.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <sstream>
#include <iomanip>
#include <algorithm> // Для std::any_of и std::find_if

// Функция для чтения событий из файла
std::vector<Event> readEvents(std::ifstream& infile) {
    std::vector<Event> events;
    std::string line;
    std::getline(infile, line);  // Пропускаем текущую строку

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        Event event;
        iss >> event.time >> event.id >> event.client;
        if (event.id == 2) {
            iss >> event.table;
        }
        if (!isValidTime(event.time)) {
            std::cerr << "Invalid time format in event: " << line << std::endl;
            exit(1);
        }
        events.push_back(event);
    }
    return events;
}

// Функция для обработки событий
void processEvents(const std::vector<Event>& events, int tableCount, const std::string& startTime, const std::string& endTime) {
    std::unordered_map<std::string, std::string> clientStatus;
    std::unordered_map<int, std::string> tableStatus;
    std::queue<std::string> waitQueue;
    std::vector<Event> outputEvents;
    std::unordered_map<int, int> tableRevenue;
    std::unordered_map<int, int> tableUsage;

    for (int i = 1; i <= tableCount; ++i) {
        tableRevenue[i] = 0;
        tableUsage[i] = 0;
    }

    for (const auto& event : events) {
        if (event.time < startTime || event.time > endTime) {
            outputEvents.push_back({event.time, 13, "NotOpenYet"});
            continue;
        }

        if (event.id == 1) {  // Клиент пришел
            if (clientStatus.find(event.client) != clientStatus.end()) {
                outputEvents.push_back({event.time, 13, "YouShallNotPass"});
            } else {
                clientStatus[event.client] = "arrived";
                outputEvents.push_back(event);
            }
        } else if (event.id == 2) {  // Клиент сел за стол
            if (clientStatus.find(event.client) == clientStatus.end()) {
                outputEvents.push_back({event.time, 13, "ClientUnknown"});
            } else if (tableStatus.find(event.table) != tableStatus.end()) {
                outputEvents.push_back({event.time, 13, "PlaceIsBusy"});
            } else {
                if (clientStatus[event.client] == "waiting") {
                    clientStatus[event.client] = "seated";
                    waitQueue.pop();
                }
                tableStatus[event.table] = event.client;
                clientStatus[event.client] = "seated";
                outputEvents.push_back(event);
            }
        } else if (event.id == 3) {  // Клиент ожидает
            if (waitQueue.size() >= static_cast<size_t>(tableCount)) {
                outputEvents.push_back({event.time, 11, event.client});
            } else if (std::none_of(tableStatus.begin(), tableStatus.end(), [](const auto& pair) { return pair.second.empty(); })) {
                outputEvents.push_back({event.time, 13, "ICanWaitNoLonger!"});
            } else {
                waitQueue.push(event.client);
                clientStatus[event.client] = "waiting";
                outputEvents.push_back(event);
            }
        } else if (event.id == 4) {  // Клиент ушел
            if (clientStatus.find(event.client) == clientStatus.end()) {
                outputEvents.push_back({event.time, 13, "ClientUnknown"});
            } else {
                clientStatus.erase(event.client);
                auto it = std::find_if(tableStatus.begin(), tableStatus.end(), [&event](const auto& pair) { return pair.second == event.client; });
                if (it != tableStatus.end()) {
                    int table = it->first;
                    tableStatus.erase(it);
                    int duration = roundUpHours(startTime, event.time);
                    tableRevenue[table] += duration * 10; // Замените 10 на вашу ставку
                    tableUsage[table] += duration;
                    if (!waitQueue.empty()) {
                        std::string nextClient = waitQueue.front();
                        waitQueue.pop();
                        tableStatus[table] = nextClient;
                        clientStatus[nextClient] = "seated";
                        outputEvents.push_back({event.time, 12, nextClient, table});
                    }
                }
                outputEvents.push_back(event);
            }
        }
    }

    // Завершение рабочего дня
    for (const auto& client_status : clientStatus) {
        const std::string& client = client_status.first;
        const std::string& status = client_status.second;
        if (status == "seated") {
            auto it = std::find_if(tableStatus.begin(), tableStatus.end(), [&client](const auto& pair) { return pair.second == client; });
            if (it != tableStatus.end()) {
                int table = it->first;
                tableStatus.erase(it);
                int duration = roundUpHours(startTime, endTime);
                tableRevenue[table] += duration * 10; // Замените 10 на вашу ставку
                tableUsage[table] += duration;
            }
            outputEvents.push_back({endTime, 11, client});
        }
    }

    // Вывод результатов
    std::cout << startTime << std::endl;
    for (const auto& e : outputEvents) {
        std::cout << e.time << " " << e.id << " " << e.client;
        if (e.id == 2 || e.id == 12) std::cout << " " << e.table;
        std::cout << std::endl;
    }
    std::cout << endTime << std::endl;

    for (const auto& table_revenue : tableRevenue) {
        int table = table_revenue.first;
        int revenue = table_revenue.second;
        int usage = tableUsage[table];
        std::cout << table << " " << revenue << " " << std::setw(2) << std::setfill('0') << (usage / 60) << ":" << std::setw(2) << std::setfill('0') << (usage % 60) << std::endl;
    }
}
