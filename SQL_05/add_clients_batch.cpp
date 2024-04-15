#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <map>
//#include <forward_list>
#include <memory>
#include <pqxx/pqxx>

#include "consts.h"
#include "connection_info.h"
#include "clients_package.h"
#include "insert_exception_structure.h"
#include "attribute.h"
#include "db_manager.h"
#include "pick_attribute.h" // ?
#include "check_phone_number.h"
#include "log_event.h"
#include "csv_parser.h"

#include "add_clients_batch.h"

void add_clients_batch(db_manager& dbm, std::string filename)
{
    const std::map<std::string, attribute::attribute> csv_headers{
        { "first_name", attribute::first_name },
        { "last_name", attribute::last_name },
        { "email", attribute::email },
        { "phone_number", attribute::phone_number },
    };
    
    csv_parser<attribute::attribute> parser(filename, csv_headers, consts::csv_separating_sing);
    
    if (!parser.is_valid())
    {
        parser.test_errors();
    }
    
    clients_package pkg;
    
    int line_number = 0;
    
    while (!parser.eof())
    {
        line_number++;
        
        std::map<attribute::attribute, std::string> current_line;
        current_line = parser.read_line();
        
        if (!current_line.empty())
        {
            std::string first_name = "";
            std::string last_name = "";
            std::string email = "";
            unsigned long long phone_number = 0;
            
            for (auto it = current_line.begin(); it != current_line.end(); it++)
            {
                switch (it->first)
                {
                case attribute::first_name:
                    first_name = it->second;
                    break;
                case attribute::last_name:
                    last_name = it->second;
                    break;
                case attribute::email:
                    email = it->second;
                    break;
                case attribute::phone_number:
                    if (it->second == "")
                    {
                        break;
                    }
                    
                    bool wrong_ph_number = false;
                    try
                {
                    phone_number = std::stoull(it->second);
                }
                    catch (const std::exception& bad_cast)
                    {
                        wrong_ph_number = true;
                    }
                    if (wrong_ph_number || !check_phone_number(phone_number))
                    {
                        throw std::runtime_error("Exception while reading line #" + std::to_string(line_number) + " (" + filename + "): wrong phone number");
                    }
                    
                    break;
                }
            }
            
            (pkg.package).push_back(std::make_tuple(first_name, last_name, email, phone_number));
        }
    }
    
    clients_package bad_input;
    auto it_client = (pkg.package).begin();
    int break_cntr = 0; // TEMPORARY FEATURE!
    while (it_client != pkg.package.end())
    {
        break_cntr++; // TEMPORARY FEATURE!
        if (break_cntr >= 50) // TEMPORARY FEATURE!
        { // TEMPORARY FEATURE!
            throw std::runtime_error("add_clients_batch: endless while-loop"); // TEMPORARY FEATURE!
        } // TEMPORARY FEATURE!
        
        try
        {
            dbm.add_client(pkg);
            it_client++;
        }
        catch (const insert_exception_structure& ies)
        {
            log_event(ies.what);
            (bad_input.package).push_back(std::move(*(ies.where)));
        }
        catch (const std::exception &other_ex)
        {
            std::string what = other_ex.what();
            throw std::runtime_error("Unexpected exception: " + what);
        }
    }
    
    if (!bad_input.package.empty())
    {
        std::string bad_emails = "";
        for (auto tuple : bad_input.package)
        {
            bad_emails += std::get<2>(tuple);
            bad_emails += " ";
        }
        throw std::runtime_error("Cannot add clients with following email addresses: " + bad_emails);
    }
}
