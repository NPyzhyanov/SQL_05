#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <memory>
#include <functional>
#include <stdexcept>

template <class EssenceEnumType>
class csv_parser
{
public:
    csv_parser(std::string filename, std::map<std::string, EssenceEnumType> headers_translation_rule, const char separating_sing)
    {
        separator = separating_sing;
        
        runtime_error_log = "";
        logic_error_log = "";
        
        p_file = std::make_unique<std::ifstream>(filename);
        
        translation_rule = std::move(headers_translation_rule);
        
        columns_headers_iterator = columns_headers.begin();
        
        headers_got = false;
        if (p_file->is_open())
        {
            std::string first_line = "";
            *p_file >> first_line;
            
            bool (csv_parser<EssenceEnumType>::*arg)(const std::string&);
            arg = &csv_parser::find_header;
            headers_got = parse_string(first_line, std::bind(&csv_parser::find_header, this, std::placeholders::_1));
        }
        else
        {
            runtime_error_log += "| file is not open |";
        }
        
        auto col_head_it = columns_headers.begin();
        while (col_head_it != columns_headers.end())
        {
            last_line_values.insert(std::pair<EssenceEnumType, std::string>(*col_head_it, ""));
            col_head_it++;
        }
    }
    
    ~csv_parser() { p_file->close(); }
    
    csv_parser(const csv_parser&) = delete;
    csv_parser& operator=(const csv_parser&) = delete;
    csv_parser(csv_parser&&) = delete;
    csv_parser& operator=(csv_parser&&) = delete;
    
    bool is_valid() { return (p_file->is_open() && headers_got); }
    
    void test_errors()
    {
        if (!logic_error_log.empty())
        {
            throw std::logic_error(logic_error_log);
        }
        if (!runtime_error_log.empty())
        {
            std::string last_errors = runtime_error_log;
            runtime_error_log = "";
            throw std::runtime_error(last_errors);
        }
    }
    
    bool eof() { return p_file->eof(); }
    
    std::optional<std::map<EssenceEnumType, std::string>> read_line()
    {
        if (!is_valid())
        {
            test_errors();
        }
        
        if (p_file->eof())
        {
            return {};
        }
        
        std::string line = "";
        *p_file >> line;
        
        if (line.empty())
        {
            return {};
        }
        
        columns_headers_iterator = columns_headers.begin();
        
        bool line_read = parse_string(line, std::bind(&csv_parser::put_value, this, std::placeholders::_1));
        if (!line_read)
        {
            test_errors();
        }
        
        return last_line_values;
    }
    
    std::map<EssenceEnumType, std::string> get_last_line()
    {
        return last_line_values;
    }
    
private:
    std::unique_ptr<std::ifstream> p_file;
    std::vector<EssenceEnumType> columns_headers;
    typename std::vector<EssenceEnumType>::iterator columns_headers_iterator;
    char separator;
    bool headers_got;
    
    std::map<std::string, EssenceEnumType> translation_rule;
    std::map<EssenceEnumType, std::string> last_line_values;
    
    std::string runtime_error_log;
    std::string logic_error_log;
    
    bool parse_string(const std::string &line, std::function<bool(const std::string&)> collateral_action)
    {
        bool read_successfully = false;
        bool error_state = false;
        bool correct_input_data = true;
        
        bool line_read = false;
        const size_t length = line.length();
        if (length == 0)
        {
            line_read = true;
        }
        
        size_t current_position = 0;
        while (!line_read)
        {
            size_t separator_position = line.find(separator, current_position);
            if (separator_position == std::string::npos)
            {
                read_successfully = true;
                line_read = true;
            }
            else if (separator_position >= length || separator_position < current_position)
            {
                error_state = true;
                bool err1 = (separator_position >= length);
                bool err2 = (separator_position < current_position);
                logic_error_log += "This csv-parser has an algorithmic defect. Just do not use it. ";
                line_read = true;
                break;
            }
            
            const std::string extracted_word = line.substr(current_position, separator_position - current_position);
            
            correct_input_data &= collateral_action(extracted_word);
            if (!correct_input_data)
            {
                break;
            }
            
            current_position = separator_position + 1;
            if (current_position >= length)
            {
                read_successfully = true;
                line_read = true;
            }
        }
        
        return (read_successfully && !error_state && correct_input_data);
    }
    
    bool find_header(const std::string &extracted_word)
    {
        bool successfully_found = false;
        
        auto it = translation_rule.find(extracted_word);
        if (it != translation_rule.end())
        {
            successfully_found = true;
            auto new_it = columns_headers.insert(columns_headers_iterator, it->second);
            columns_headers_iterator = new_it + 1;
        }
        else
        {
            std::string error_msg = "| cannot find header '" + extracted_word + "' |";
            runtime_error_log += error_msg;
        }
        
        return successfully_found;
    }
    
    bool put_value(const std::string &extracted_word)
    {
        auto key = *columns_headers_iterator;
        last_line_values.at(key) = extracted_word;
        ++columns_headers_iterator;
        
        return true;
    }
};

#endif // CSV_PARSER_H
