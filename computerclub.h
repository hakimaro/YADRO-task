#ifndef COMPUTERCLUB_H
#define COMPUTERCLUB_H

#include <queue>
#include <vector>
#include <map>
#include <list>
#include <cinttypes>
#include "event.h"

using namespace std;

struct Client {
    Time start_time;
    string id;

    Client(Time time, string id);
};

struct Table {
    Client current_client = Client(Time(), string());
    uint32_t gain = 0;
    Time work_time;
};

class ComputerClub
{
public:
    ComputerClub();
    ComputerClub(uint16_t count_of_tables, Time start_time, Time end_time, uint16_t price_per_hour);

    void start_working(queue<IncomingEvent> events);
    uint16_t count_of_tables() const;
private:
    /* Методы */
    void process_client_arrived(const IncomingEvent& event);
    void process_client_sat_down(const IncomingEvent& event);
    void process_client_waiting(const IncomingEvent& event);
    void process_client_gone(const IncomingEvent& event);

    int find_client_at_table(string id);
    void put_client_at_table(const int pos, const IncomingEvent& event);
    void remove_client_from_table(const int pos, const IncomingEvent& event);

    /* Поля */
    uint16_t m_count_of_tables;
    Time m_start_time;
    Time m_end_time;
    uint16_t m_price_per_hour;

    list<string> m_clients_inside; // Клиент, который вошел, но еще не в очереди
    vector<Table> m_tables;        // Столы
    list<string> m_queue;          // Очередь ожидания
};

#endif // COMPUTERCLUB_H
