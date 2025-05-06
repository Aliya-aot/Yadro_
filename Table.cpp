#include "Table.h"
#include <stdexcept> 

Table::Table(int id, int cost_per_hour) :
    id_(id),
    is_busy_(false),
    client_name_(""),
    start_use_time_(),
    total_minutes_occupied_(0),
    revenue_(0),
    cost_per_hour_(cost_per_hour) {}

int Table::get_id() const { return id_; }
bool Table::is_occupied() const { return is_busy_; }
const std::string& Table::get_client_name() const { return client_name_; }
int Table::get_revenue() const { return revenue_; }
int Table::get_total_minutes() const { return total_minutes_occupied_; }

void Table::occupy(const std::string& client_name, const Time& time) {
    if (is_busy_) {
        throw std::logic_error("Internal error: Trying to occupy a busy table " + std::to_string(id_));
    }
    is_busy_ = true;
    client_name_ = client_name;
    start_use_time_ = time;
}

void Table::release(const Time& time) {
    if (!is_busy_) {
        return; 
    }
    int duration = time.minutes_since(start_use_time_);
    if (duration < 0) duration = 0; 

    int payment = calculate_payment(duration);
    revenue_ += payment;
    total_minutes_occupied_ += duration;

    is_busy_ = false;
    client_name_ = "";
}

int Table::calculate_payment(int duration_minutes) const {
    if (duration_minutes <= 0) return 0;
    return static_cast<int>(std::ceil(static_cast<double>(duration_minutes) / 60.0)) * cost_per_hour_;
}

std::string Table::format_duration(int total_minutes) {
    std::ostringstream oss;
    int hours = total_minutes / 60;
    int minutes = total_minutes % 60;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes;
    return oss.str();
}