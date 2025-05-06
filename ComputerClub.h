#pragma once
#include "Time.h"
#include "Table.h"
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <list>    
#include <sstream> 
#include <stdexcept> 
#include <algorithm> 
#include <regex>   


bool is_valid_client_name(const std::string& name);


class ComputerClub {
private:
    int num_tables_;
    Time open_time_;
    Time close_time_;
    int cost_per_hour_;
    std::vector<Table> tables_;
    std::map<std::string, int> client_locations_;
    std::set<std::string> clients_present_;
    std::queue<std::string> waiting_queue_;
    std::list<std::string> output_log_;
    Time last_event_time_;

    void add_output(const std::string& line);
    void add_generated_event(const Time& t, int event_id, const std::string& body);
    void release_table_and_seat_next(int table_id, const Time& time);

    void handle_client_arrival(const Time& time, const std::string& client_name);
    void handle_client_sit(const Time& time, const std::string& client_name, int table_id);
    void handle_client_wait(const Time& time, const std::string& client_name);
    void handle_client_leave(const Time& time, const std::string& client_name);

public:
    ComputerClub(int num_tables, const Time& open, const Time& close, int cost);

    const Time& get_open_time() const; 
    const Time& get_close_time() const; 
    const Time& get_last_event_time() const;
    void set_last_event_time(const Time& t);

    void process_event(const Time& time, int event_id, const std::string& client_name, int table_id_event = 0);
    void process_end_of_day();
    void log_incoming_event(const std::string& line);
    void print_results(std::ostream& os) const;
};