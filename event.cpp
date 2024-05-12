#include "event.h"

Time::Time() :
    hour(0), minute(0)
{

}

Time::Time(uint8_t hour, uint8_t minute) :
    hour(hour), minute(minute)
{

}

uint16_t Time::to_minutes() const
{
    return 60*hour + minute;
}

Time Time::from_minutes(uint16_t minutes)
{
    return Time(minutes / 60, minutes % 60);
}

Time Time::operator-(const Time &time)
{
    return from_minutes(time.to_minutes() - to_minutes());
}

Time Time::operator+(const Time &time)
{
    return from_minutes(time.to_minutes() + to_minutes());
}

bool Time::operator<(Time &time) const
{
    return to_minutes() < time.to_minutes();
}

bool Time::operator>(Time &time) const
{
    return to_minutes() > time.to_minutes();
}

ostream &operator<<(ostream &os, IncomingEvent &event)
{
    os << event.time // Запись времени
       << " "
       << static_cast<int>(event.event) // Запись типа события
       << " "
       << event.id; // Запись имени

    if (event.place) {
        os << " " << event.place.value();
    }

    return os;
}

ostream& operator<<(ostream& os, Time& time)
{
    os << static_cast<int>(time.hour / 10)
       << static_cast<int>(time.hour % 10)
       << ":"
       << static_cast<int>(time.minute / 10)
       << static_cast<int>(time.minute % 10);

    return os;
}

ostream& operator<<(ostream& os, OutgoingEvent& event) {
    os << event.time // Запись времени
       << " "
       << static_cast<int>(event.event) // Запись типа события
       << " "
       << event.message; // Запись имени

    return os;
}

IncomingEvent::IncomingEvent(uint8_t hours, uint8_t minutes, IncomingEventType event, string id, optional<uint16_t> place) :
    time(hours, minutes), event(event), id(id), place(place)
{

}

IncomingEvent::IncomingEvent(Time time, IncomingEventType event, string id, optional<uint16_t> place) :
    time(time), event(event), id(id), place(place)
{

}

OutgoingEvent::OutgoingEvent(uint8_t hours, uint8_t minutes, OutgoingEventType event, string message) :
    time(hours, minutes), event(event), message(message)
{

}

OutgoingEvent::OutgoingEvent(Time time, OutgoingEventType event, string message) :
    time(time), event(event), message(message)
{

}
