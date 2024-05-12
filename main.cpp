#include <iostream>
#include "computerclub.h"
#include <fstream>
#include <stdio.h>
#include <queue>
#include <regex>

using namespace std;
/* Для числа */
const regex r_number("^[0-9]+$");
/* Для времени работы клуба */
const regex r_time_twice("^(([01][0-9]|2[0-3]):([0-5][0-9]))\\s(([01][0-9]|2[0-3]):([0-5][0-9]))$");
/* Для времени */
const regex r_time("^([01][0-9]|2[0-3]):([0-5][0-9])$");
/* Для обработки события*/
const regex r_event(R"(^(([01][0-9]|2[0-3]):[0-5][0-9])\s([1-4])\s([a-z0-9_-]+)\s?([0-9]+)?$)");

Time to_time(string str) {
    if (!regex_match(str, r_time)) {
        throw str;
    }

    smatch time_match;
    regex_search(str, time_match, r_time);

    int hours = stoi(time_match[1]);
    int minutes = stoi(time_match[2]);

    return Time(hours, minutes);
}

IncomingEvent to_event(string str) {
    if (!regex_match(str, r_event)) {
        throw str;
    }

    smatch event_match;
    regex_search(str, event_match, r_event);

    Time time = to_time(event_match[1]);
    int event_type = stoi(event_match[3]);
    string id = event_match[4];
    optional<uint16_t> place;
    if (event_match[5] != string()) {
        place = stoi(event_match[5]);
    }

    return IncomingEvent(time, (IncomingEventType) event_type, id, place);
}

ComputerClub read_club_parameters(ifstream &in) {
    string count_line;
    getline(in, count_line);
    if (!regex_match(count_line, r_number)) {
        throw count_line;
    }
    int count = stoi(count_line);

    string two_times_line;
    getline(in, two_times_line);
    if (!regex_match(two_times_line, r_time_twice)) {
        throw two_times_line;
    }
    smatch times_match;
    regex_search(two_times_line, times_match, r_time_twice);
    Time start_time = to_time(times_match[1]);
    Time end_time = to_time(times_match[4]);

    string price_per_hour_line;
    getline(in, price_per_hour_line);
    if (!regex_match(price_per_hour_line, r_number)) {
        throw price_per_hour_line;
    }
    int price_per_hour = stoi(price_per_hour_line);

    return ComputerClub(count, start_time, end_time, price_per_hour);
}

queue<IncomingEvent> read_events(ifstream &in, int count_of_tables) {
    string event_line;
    queue<IncomingEvent> events;

    Time last_time = Time();
    while(getline(in, event_line)) {
        IncomingEvent event = to_event(event_line);
        if (event.time < last_time) {
            throw event_line;
        }
        if (event.place && (event.place.value() <= 0 || event.place.value() > count_of_tables)) {
            throw event_line;
        }
        events.push(to_event(event_line));
        last_time = event.time;
    }

    return events;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Write direction to file!" << endl;
        return 0;
    }

    char *filename = argv[1];
    ifstream file(filename, std::ios_base::binary);
    if (!file) {
        cout << "Couldn't open file!" << endl;
        return 0;
    }

    try {
        ComputerClub club = read_club_parameters(file);
        queue<IncomingEvent> events = read_events(file, club.count_of_tables());
        club.start_working(events);

    } catch (string str) {
        cout << str << endl;
    }
    return 0;
}
