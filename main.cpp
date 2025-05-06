#include "ComputerClub.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept> 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    int line_number = 0; 

    try {
        //Чтение количества столов
        line_number++;
        if (!std::getline(input_file, line)) throw std::runtime_error("Error in line " + std::to_string(line_number) + ": Missing number of tables");
        int num_tables;
        try {
            size_t pos;
            num_tables = std::stoi(line, &pos);
            if (pos != line.length() || num_tables <= 0) throw std::runtime_error(""); 
        }
        catch (const std::exception&) {
            throw std::runtime_error(""); 
        }


        //Чтение времени работы
        line_number++;
        if (!std::getline(input_file, line)) throw std::runtime_error("Error in line " + std::to_string(line_number) + ": Missing working hours");
        Time open_time, close_time;
        std::stringstream ss_time(line);
        std::string open_str, close_str, extra_time_str;
        if (!(ss_time >> open_str >> close_str) || ss_time >> extra_time_str ||
            !Time::try_parse(open_str, open_time) ||
            !Time::try_parse(close_str, close_time) ||
            open_time >= close_time) {
            throw std::runtime_error("");
        }

        //Чтение стоимости часа
        line_number++;
        if (!std::getline(input_file, line)) throw std::runtime_error("Error in line " + std::to_string(line_number) + ": Missing cost per hour");
        int cost_per_hour;
        try {
            size_t pos;
            cost_per_hour = std::stoi(line, &pos);
            if (pos != line.length() || cost_per_hour <= 0) throw std::runtime_error("");
        }
        catch (const std::exception&) {
            throw std::runtime_error("");
        }

        ComputerClub club(num_tables, open_time, close_time, cost_per_hour);

        std::string last_valid_line_for_error_output = line; 

        while (std::getline(input_file, line)) {
            line_number++;
            last_valid_line_for_error_output = line; 
            std::string original_line = line;

            std::stringstream ss_event(line);
            std::string event_time_str, client_name_str, body_part1_str, extra_event_str;

            int event_id_val = 0;
            int table_id_event_val = 0;
            Time event_time_val;

            if (!(ss_event >> event_time_str >> event_id_val >> client_name_str)) {
                throw std::runtime_error("");
            }
            if (!Time::try_parse(event_time_str, event_time_val)) {
                throw std::runtime_error("");
            }
            if (!is_valid_client_name(client_name_str)) { 
                throw std::runtime_error("");
            }
            if (event_time_val < club.get_last_event_time()) { 
                throw std::runtime_error("");
            }


            bool format_ok = true;
            switch (event_id_val) {
            case 1: case 3: case 4:
                if (ss_event >> extra_event_str) format_ok = false;
                break;
            case 2:
                try {
                    if (!(ss_event >> body_part1_str)) {
                        format_ok = false;
                    }
                    else {
                        size_t pos;
                        table_id_event_val = std::stoi(body_part1_str, &pos);
                        if (pos != body_part1_str.length() || table_id_event_val <= 0 || table_id_event_val > num_tables) { 
                            format_ok = false;
                        }
                        if (ss_event >> extra_event_str) format_ok = false;
                    }
                }
                catch (...) {
                    format_ok = false;
                }
                break;
            default: 
                format_ok = false;
            }

            if (!format_ok) {
                throw std::runtime_error("");
            }

            club.set_last_event_time(event_time_val);

            club.log_incoming_event(original_line);
            club.process_event(event_time_val, event_id_val, client_name_str, table_id_event_val);
        }

        club.process_end_of_day();
        club.print_results(std::cout);

    }
    catch (const std::runtime_error& e) {
        std::cout << line << std::endl; 
        return 1; 
    }

    return 0;
}