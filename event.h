#ifndef EVENT_H
#define EVENT_H

#include <cinttypes>
#include <string>
#include <iostream>
#include <optional>
#include <fstream>

using namespace std;

enum class IncomingEventType {
    ClientArrived = 1, // Клиент пришел
    ClientSatDown = 2, // Клиент сел за стол
    ClientWaiting = 3, // Клиент ожидает
    ClientGone = 4,
};

enum class OutgoingEventType {
    ClientGone = 11,
    ClientSatDown = 12,
    Error = 13
};

struct Time {
    uint8_t hour;
    uint8_t minute;

    Time();
    Time(uint8_t hour, uint8_t minute);
    uint16_t to_minutes() const;
    Time from_minutes(uint16_t minutes);
    Time operator-(const Time& time);
    Time operator+(const Time& time);
    bool operator<(Time& time) const;
    bool operator>(Time& time) const;
};

struct IncomingEvent {
    /* Время события */
    Time time = Time();
    /* Тип события */
    IncomingEventType event = IncomingEventType::ClientArrived;
    /* Имя клиента */
    string id = string();
    /* Место, куда хочет сесть клиент (не всегда используется) */
    optional<uint16_t> place;

    IncomingEvent() = default;
    IncomingEvent(uint8_t hours, uint8_t minutes, IncomingEventType event, string id, optional<uint16_t> place = optional<uint16_t>());
    IncomingEvent(Time time, IncomingEventType event, string id, optional<uint16_t> place = optional<uint16_t>());
};

struct OutgoingEvent {
    /* Время события */
    Time time = Time();
    /* Тип события */
    OutgoingEventType event = OutgoingEventType::Error;
    /* Тело сообщения (имя клиента или ошибка) */
    string message = string();

    OutgoingEvent() = default;
    OutgoingEvent(uint8_t hours, uint8_t minutes, OutgoingEventType event, string id);
    OutgoingEvent(Time time, OutgoingEventType event, string id);
};

ostream& operator<<(ostream& os, IncomingEvent& event);
ostream& operator<<(ostream& os, Time& time);
ostream& operator<<(ostream& os, OutgoingEvent& event);


#endif // EVENT_H
