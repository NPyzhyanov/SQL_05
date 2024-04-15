#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <memory>
#include <pqxx/pqxx>

#include "connection_info.h"
#include "clients_package.h"
#include "attribute.h"
#include "db_manager.h"

void db_manager::setup_connection()
{
    do
    {
        std::system("clear");
        
        try
        {
            std::cout << "Enter 'y' or '1' if you want to continue with default connection parameters: host=localhost port=5432." << std::endl;
            std::cout << "Enter 'q' or '0' if you want to exit." << std::endl;
            std::cout << "Enter any other key to continue with other connection parameters." << std::endl;
            std::cout << "Do you want to continue with default connection parameters? ";
            std::string conn_params_as_default_in;
            std::cin >> conn_params_as_default_in;
            
            connection_set_up = false;
            
            if (conn_params_as_default_in == "q" || conn_params_as_default_in == "0")
            {
                break;
            }
            else if (conn_params_as_default_in == "y" || conn_params_as_default_in == "1")
            {
                conn_info.host = "localhost";
                conn_info.port = "5432";
            }
            else
            {
                std::cout << "host=";
                std::string host_in;
                std::cin >> host_in;
                conn_info.host = host_in;
                
                std::cout << "port=";
                std::string port_in;
                std::cin >> port_in;
                conn_info.port = port_in;
            }
            
            std::cout << "Enter database's name: ";
            std::string dbname_in;
            std::cin >> dbname_in;
            conn_info.dbname = dbname_in;
            
            std::cout << "Enter username: ";
            std::string username_in;
            std::cin >> username_in;
            conn_info.username = username_in;
            
            std::cout << "Enter password: ";
            std::string password;
            std::cin >> password;
            
            std::string connection_argument = build_connection_argument(password);
            
            if (p_connection == nullptr)
            {
                p_connection = std::make_unique<pqxx::connection>(connection_argument);
            }
            else
            {
                auto new_connection = std::make_unique<pqxx::connection>(connection_argument);
                p_connection.swap(new_connection);
            }
            
            std::system("clear");
            std::cout << "The connection set up successfully\n" << std::endl;
            connection_set_up = true;
        }
        catch(const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            std::cout << "Please, try again!\n" << std::endl;
        }
    }
    while (!connection_set_up);
}
