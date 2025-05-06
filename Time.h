#pragma once
#include <string>
#include <iostream> 
#include <iomanip>  

class Time {
public:
    int hour;
    int minute;
    int total_minutes;

    Time(int h = 0, int m = 0);
    explicit Time(const std::string& time_str);

    static bool try_parse(const std::string& time_str, Time& result);

    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    bool operator>=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator==(const Time& other) const;
    bool operator!=(const Time& other) const;

    int minutes_since(const Time& start_time) const;

    friend std::ostream& operator<<(std::ostream& os, const Time& t);

private:
    void parse_string(const std::string& time_str);
};