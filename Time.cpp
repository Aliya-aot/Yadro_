#include "Time.h"
#include <stdexcept> 
#include <string>   

Time::Time(int h, int m) : hour(h), minute(m) {
    total_minutes = h * 60 + m;
}

Time::Time(const std::string& time_str) {
    parse_string(time_str);
}


bool Time::try_parse(const std::string& time_str, Time& result) {
    if (time_str.length() != 5 || time_str[2] != ':') {
        return false;
    }
    int h, m;
    try {
        size_t pos_h, pos_m; // Используем разные переменные для отслеживания позиций
        h = std::stoi(time_str.substr(0, 2), &pos_h);
        if (pos_h != 2) return false;
        m = std::stoi(time_str.substr(3, 2), &pos_m);
        if (pos_m != 2) return false;
    }
    catch (const std::exception&) {
        return false;
    }
    if (h < 0 || h > 23 || m < 0 || m > 59) {
        return false;
    }
    result = Time(h, m);
    return true;
}

bool Time::operator<(const Time& other) const { return total_minutes < other.total_minutes; }
bool Time::operator<=(const Time& other) const { return total_minutes <= other.total_minutes; }
bool Time::operator>=(const Time& other) const { return total_minutes >= other.total_minutes; }
bool Time::operator>(const Time& other) const { return total_minutes > other.total_minutes; }
bool Time::operator==(const Time& other) const { return total_minutes == other.total_minutes; }
bool Time::operator!=(const Time& other) const { return total_minutes != other.total_minutes; }


int Time::minutes_since(const Time& start_time) const {
    return this->total_minutes - start_time.total_minutes;
}

std::ostream& operator<<(std::ostream& os, const Time& t) {
    os << std::setfill('0') << std::setw(2) << t.hour << ":"
        << std::setfill('0') << std::setw(2) << t.minute;
    return os;
}

void Time::parse_string(const std::string& time_str) {
    if (!Time::try_parse(time_str, *this)) {
        throw std::invalid_argument("Invalid time format: " + time_str);
    }
}