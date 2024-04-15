#include <iostream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <memory>
#include <pqxx/pqxx>

#include "connection_info.h"
#include "clients_package.h"
#include "attribute.h"
#include "db_manager.h"

#include "print_clients_package.h"

void print_clients_package(db_manager &dbm, attribute::attribute search_criterion, std::string value)
{
    try
    {
        clients_package pkg = dbm.find_client(search_criterion, value);
        std::cout << "first name\tlast name\temail\tphone number" << std::endl;
        for (auto [first_name, last_name, email, phone_number] : pkg.package)
        {
            std::cout << first_name << "\t" << last_name << "\t" << email << "\t" << std::to_string(phone_number) << std::endl;
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "Cannot find clients. " << ex.what() << std::endl;
    }
}
