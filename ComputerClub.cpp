#include "ComputerClub.h"

bool is_valid_client_name(const std::string& name) {
    static const std::regex valid_name_regex("^[a-z0-9_-]+$");
    return std::regex_match(name, valid_name_regex);
}


ComputerClub::ComputerClub(int num_tables, const Time& open, const Time& close, int cost) :
    num_tables_(num_tables),
    open_time_(open),
    close_time_(close),
    cost_per_hour_(cost),
    last_event_time_(0, 0) 
{
    if (num_tables <= 0 || cost <= 0 || open >= close) {

        throw std::logic_error("Invalid club configuration parameters passed to constructor.");
    }
    tables_.reserve(num_tables_);
    for (int i = 0; i < num_tables_; ++i) {
        tables_.emplace_back(i + 1, cost_per_hour_);
    }
    std::ostringstream oss_open;
    oss_open << open_time_;
    add_output(oss_open.str());
}

const Time& ComputerClub::get_open_time() const { return open_time_; }
const Time& ComputerClub::get_close_time() const { return close_time_; }
const Time& ComputerClub::get_last_event_time() const { return last_event_time_; }
void ComputerClub::set_last_event_time(const Time& t) { last_event_time_ = t; }


void ComputerClub::add_output(const std::string& line) {
    output_log_.push_back(line);
}

void ComputerClub::add_generated_event(const Time& t, int event_id, const std::string& body) {
    std::ostringstream oss;
    oss << t << " " << event_id << " " << body;
    add_output(oss.str());
}

void ComputerClub::release_table_and_seat_next(int table_id, const Time& time) {
    if (table_id <= 0 || table_id > num_tables_) return; // Защита
    Table& table = tables_[table_id - 1];
    if (!table.is_occupied()) return;

    bool was_occupied_by_leaving_client = table.is_occupied();
    std::string leaving_client_name = table.get_client_name(); 

    if (was_occupied_by_leaving_client) {
        table.release(time); 
    }


    if (!waiting_queue_.empty()) {

        if (!table.is_occupied()) {
            std::string next_client = waiting_queue_.front();
            waiting_queue_.pop();

            if (clients_present_.count(next_client)) {
                table.occupy(next_client, time);
                client_locations_[next_client] = table_id; 
                add_generated_event(time, 12, next_client + " " + std::to_string(table_id));
            }
        }
    }
}

void ComputerClub::process_event(const Time& time, int event_id, const std::string& client_name, int table_id_event) {
    try {

        switch (event_id) {
        case 1:
            handle_client_arrival(time, client_name);
            break;
        case 2:
            if (table_id_event <= 0 || table_id_event > num_tables_) {
                throw std::runtime_error("PlaceIsBusy"); 
            }
            handle_client_sit(time, client_name, table_id_event);
            break;
        case 3:
            handle_client_wait(time, client_name);
            break;
        case 4:
            handle_client_leave(time, client_name);
            break;
        default:
            throw std::logic_error("Internal error: Unknown event ID in process_event");
        }
    }
    catch (const std::runtime_error& e) {
        add_generated_event(time, 13, e.what());
    }
}


void ComputerClub::handle_client_arrival(const Time& time, const std::string& client_name) {
    if (time < open_time_ || time >= close_time_) { 
        throw std::runtime_error("NotOpenYet");
    }
    if (clients_present_.count(client_name)) {
        throw std::runtime_error("YouShallNotPass");
    }
    clients_present_.insert(client_name);

}

void ComputerClub::handle_client_sit(const Time& time, const std::string& client_name, int table_id) {
    if (!clients_present_.count(client_name)) {
        throw std::runtime_error("ClientUnknown");
    }
    Table& target_table = tables_[table_id - 1]; 

    if (target_table.is_occupied()) {
        throw std::runtime_error("PlaceIsBusy");
    }

    
    if (client_locations_.count(client_name)) { 
        int old_location = client_locations_[client_name];
        if (old_location > 0 && old_location != table_id) { 

                release_table_and_seat_next(old_location, time);
        }
        client_locations_.erase(client_name); 
    }
    target_table.occupy(client_name, time);
    client_locations_[client_name] = table_id;
}


void ComputerClub::handle_client_wait(const Time& time, const std::string& client_name) {
    if (!clients_present_.count(client_name)) {
        throw std::runtime_error("ClientUnknown");
    }

    if (client_locations_.count(client_name) && client_locations_[client_name] > 0) {

        throw std::runtime_error("ICanWaitNoLonger!"); 
    }


    for (const auto& table : tables_) {
        if (!table.is_occupied()) {
            throw std::runtime_error("ICanWaitNoLonger!");
        }
    }

    if (waiting_queue_.size() >= static_cast<size_t>(num_tables_)) {

        clients_present_.erase(client_name); 
        client_locations_.erase(client_name); 
        add_generated_event(time, 11, client_name); 
    }
    else {
        waiting_queue_.push(client_name);
        client_locations_[client_name] = 0; 
    }
}


void ComputerClub::handle_client_leave(const Time& time, const std::string& client_name) {
    if (!clients_present_.count(client_name)) {
        throw std::runtime_error("ClientUnknown");
    }

    int table_id_left = 0;
    if (client_locations_.count(client_name)) {
        table_id_left = client_locations_[client_name];

    }

    clients_present_.erase(client_name);

    if (table_id_left > 0) { 
        release_table_and_seat_next(table_id_left, time);
    }
    else if (client_locations_.count(client_name) && table_id_left == 0) { 

        std::queue<std::string> new_queue;
        while (!waiting_queue_.empty()) {
            std::string current_client = waiting_queue_.front();
            waiting_queue_.pop();
            if (current_client != client_name) {
                new_queue.push(current_client);
            }
        }
        waiting_queue_ = new_queue;
    }
    client_locations_.erase(client_name); 
}


void ComputerClub::process_end_of_day() {
    std::vector<std::string> clients_to_evict;

    for (const std::string& name : clients_present_) {
        clients_to_evict.push_back(name);
    }

    for (const std::string& name : clients_to_evict) {
        add_generated_event(close_time_, 11, name);
        if (client_locations_.count(name)) {
            int location = client_locations_[name];
            if (location > 0) { 
                Table& table = tables_[location - 1];
                if (table.is_occupied() && table.get_client_name() == name) { 
                    table.release(close_time_);
                }
            }
           
        }
    }
    clients_present_.clear();
    client_locations_.clear();
    std::queue<std::string> empty_queue;
    std::swap(waiting_queue_, empty_queue); 

    std::ostringstream oss_close;
    oss_close << close_time_;
    add_output(oss_close.str());
}

void ComputerClub::log_incoming_event(const std::string& line) {
    add_output(line);
}

void ComputerClub::print_results(std::ostream& os) const {
    for (const auto& line : output_log_) {
        os << line << std::endl;
    }
 
    for (const auto& table : tables_) {
        os << table.get_id() << " " << table.get_revenue() << " "
            << Table::format_duration(table.get_total_minutes()) << std::endl;
    }
}