#include "computerclub.h"
#include <algorithm>
#include <iostream>
#include <optional>

ComputerClub::ComputerClub()
{
    m_count_of_tables = 0;
    m_start_time = Time();
    m_end_time = Time();
    m_price_per_hour = 0;
}

ComputerClub::ComputerClub(const uint16_t count_of_tables, const Time start_time, const Time end_time, uint16_t price_per_hour)
{
    m_count_of_tables = count_of_tables;
    m_start_time = start_time;
    m_end_time = end_time;
    m_price_per_hour = price_per_hour;
}

void ComputerClub::start_working(queue<IncomingEvent> events)
{
    m_tables.clear();
    m_tables.resize(m_count_of_tables);

    cout << m_start_time << endl;
    while (!events.empty()) {
        IncomingEvent current_event = events.front();
        cout << current_event << endl;

        try {
            switch(current_event.event) {
            case IncomingEventType::ClientArrived:
                process_client_arrived(current_event);
                break;

            case IncomingEventType::ClientSatDown:
                process_client_sat_down(current_event);
                break;

            case IncomingEventType::ClientWaiting:
                process_client_waiting(current_event);
                break;

            case IncomingEventType::ClientGone:
                process_client_gone(current_event);
                break;
            }
        } catch(OutgoingEvent event){
            cout << event << endl;
        }

        events.pop();
    }

    std::vector<string> all_clients_gone;
    for(int i = 0; i < m_tables.size(); ++i) {
        if (m_tables[i].current_client.id != string()) {
            all_clients_gone.push_back(m_tables[i].current_client.id);
            remove_client_from_table(i, IncomingEvent(m_end_time, IncomingEventType::ClientGone, m_tables[i].current_client.id));
        }
    }
    for(auto client : m_clients_inside) {
        all_clients_gone.push_back(client);
    }
    for(auto client : m_queue) {
        all_clients_gone.push_back(client);
    }

    /* Сортировка в алфавитном порядке (сначала -, _, потом цифры, потом буквы) */
    sort(all_clients_gone.begin(), all_clients_gone.end());

    /* Вывод клиентов, покидающих компьютерный клуб в алфавитном порядке */
    for(int i = 0; i < all_clients_gone.size(); ++i) {
        OutgoingEvent event(m_end_time, OutgoingEventType::ClientGone, all_clients_gone[i]);
        cout << event << endl;
    }

    /* Вывод окончания времени работы */
    cout << m_end_time << endl;
    /* Вывод выручки */
    for(int i = 0; i < m_tables.size(); ++i) {
        cout << i+1 << " " << m_tables[i].gain << " " << m_tables[i].work_time << endl;
    }
}

void ComputerClub::process_client_arrived(const IncomingEvent &event)
{
    auto table_number = find_client_at_table(event.id);
    auto queue_position = find(m_queue.begin(), m_queue.end(), event.id);
    auto clients_inside_position = find(m_clients_inside.begin(), m_clients_inside.end(), event.id);

    if (table_number != -1 || queue_position != m_queue.end() || clients_inside_position != m_clients_inside.end())
    {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "YouShallNotPass");
    }

    if (event.time < m_start_time || event.time > m_end_time)
    {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "NotOpenYet");
    }

    m_clients_inside.push_back(event.id);
    return;
}

void ComputerClub::process_client_sat_down(const IncomingEvent &event)
{
    auto table_number = find_client_at_table(event.id);
    auto queue_position = find(m_queue.begin(), m_queue.end(), event.id);
    auto clients_inside_position = find(m_clients_inside.begin(), m_clients_inside.end(), event.id);

    if (m_tables[event.place.value() - 1].current_client.id != string()) {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "PlaceIsBusy");
    }
    if (table_number == -1 && queue_position == m_queue.end() && clients_inside_position == m_clients_inside.end())
    {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "ClientUnknown");
    }

    if (table_number != -1) {
        remove_client_from_table(table_number, event);
    }
    if (queue_position != m_queue.end()) {
        m_queue.erase(queue_position);
    }
    if (clients_inside_position != m_clients_inside.end()) {
        m_clients_inside.erase(clients_inside_position);
    }

    put_client_at_table(event.place.value() - 1, event);
}

void ComputerClub::process_client_waiting(const IncomingEvent &event)
{
    auto table_number = find_client_at_table(event.id);
    auto queue_position = find(m_queue.begin(), m_queue.end(), event.id);
    auto clients_inside_position = find(m_clients_inside.begin(), m_clients_inside.end(), event.id);

    if (find_client_at_table(string()) != -1) {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "ICanWaitNoLonger");
    }

    if (table_number == -1 && queue_position == m_queue.end() && clients_inside_position == m_clients_inside.end()) {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "ClientUnknown");
    }

    if (table_number != -1) {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "AlreadyAtTheTable");
    }

    if (clients_inside_position != m_clients_inside.end()) {
        m_clients_inside.erase(clients_inside_position);
    }

    if (m_queue.size() > m_count_of_tables) {
        m_clients_inside.erase(clients_inside_position);
        throw OutgoingEvent(event.time, OutgoingEventType::ClientGone, event.id);
    }

    if (queue_position == m_queue.end()) {
        m_queue.push_back(event.id);
    }
}

void ComputerClub::process_client_gone(const IncomingEvent &event)
{
    auto table_number = find_client_at_table(event.id);
    auto queue_position = find(m_queue.begin(), m_queue.end(), event.id);
    auto clients_inside_position = find(m_clients_inside.begin(), m_clients_inside.end(), event.id);

    if (table_number == -1 && queue_position == m_queue.end() && clients_inside_position == m_clients_inside.end())
    {
        throw OutgoingEvent(event.time, OutgoingEventType::Error, "ClientUnknown");
    }
    if (table_number != -1) {
        remove_client_from_table(table_number, event);
        if (!m_queue.empty()) {
            string id = m_queue.front();
            m_queue.pop_front();
            put_client_at_table(table_number, IncomingEvent(event.time, IncomingEventType::ClientSatDown, id));
            throw OutgoingEvent(event.time, OutgoingEventType::ClientSatDown, id);
        }
    }

    if (queue_position != m_queue.end()) {
        m_queue.erase(queue_position);
    }
    if (clients_inside_position != m_clients_inside.end()) {
        m_clients_inside.erase(clients_inside_position);
    }
}

int ComputerClub::find_client_at_table(string id)
{
    for(int i = 0; i < m_tables.size(); ++i) {
        if (m_tables[i].current_client.id == id) {
            return i;
        }
    }
    return -1;
}

void ComputerClub::put_client_at_table(const int pos, const IncomingEvent& event)
{
    m_tables[pos].current_client.id = event.id;
    m_tables[pos].current_client.start_time = event.time;
}

void ComputerClub::remove_client_from_table(const int pos, const IncomingEvent& event)
{
    m_tables[pos].gain += m_price_per_hour * ((m_tables[pos].current_client.start_time - event.time).hour + 1);
    m_tables[pos].current_client.id = string();
    m_tables[pos].work_time = m_tables[pos].work_time + (m_tables[pos].current_client.start_time - event.time);
}

uint16_t ComputerClub::count_of_tables() const
{
    return m_count_of_tables;
}


Client::Client(Time time, string id):
    start_time(time), id(id)
{

}
