#pragma once
#include "Time.h" 
#include <string>
#include <sstream> 
#include <iomanip> 
#include <cmath>   

class Table {
private:
    int id_;
    bool is_busy_;
    std::string client_name_;
    Time start_use_time_;
    int total_minutes_occupied_;
    int revenue_;
    int cost_per_hour_;

public:
    Table(int id, int cost_per_hour);

    int get_id() const;
    bool is_occupied() const;
    const std::string& get_client_name() const;
    int get_revenue() const;
    int get_total_minutes() const;

    void occupy(const std::string& client_name, const Time& time);
    void release(const Time& time);
    int calculate_payment(int duration_minutes) const;

    static std::string format_duration(int total_minutes);
};
