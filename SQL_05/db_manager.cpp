#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <memory>
#include <pqxx/pqxx>

#include "log_event.h"
#include "connection_info.h"
#include "clients_package.h"
#include "insert_exception_structure.h"
#include "empty_value_exception.h"
#include "attribute.h"

#include "db_manager.h"

db_manager::db_manager()
{
    p_connection = nullptr;
    connection_set_up = false;
    db_is_empty = true;
    
    setup_connection();
}

//void db_manager::setup_connection() -- described in setup_connection.cpp

std::string db_manager::build_connection_argument(std::string password)
{
    return "host=" + conn_info.host +
            " port=" + conn_info.port +
            " dbname=" + conn_info.dbname +
            " user=" + conn_info.username +
            " password=" + password;
}

void db_manager::create_db()
{
    {
        pqxx::work transaction{ *p_connection };
        transaction.exec("CREATE TABLE IF NOT EXISTS Client ("
                         "email varchar(64) PRIMARY KEY, "
                         "first_name varchar(64) NOT NULL, "
                         "last_name varchar(64) NOT NULL);");
        transaction.commit();
        log_event("Table 'Client' created");
    }
    {
        pqxx::work transaction{ *p_connection };
        transaction.exec("CREATE TABLE IF NOT EXISTS PhoneNumber ("
                         "ph_number bigint CHECK (ph_number > 79000000000 AND ph_number < 79999999999) PRIMARY KEY, "
                         "email varchar(64) REFERENCES Client(email) ON UPDATE CASCADE);");
        transaction.commit();
        log_event("Table 'PhoneNumber' created");
    }
}

void db_manager::add_client(clients_package &clients_to_add)
{
    for (auto it = clients_to_add.package.begin(); it != clients_to_add.package.end(); it++)
    {
        auto first_name = std::get<0>(*it);
        auto last_name = std::get<1>(*it);
        auto email = std::get<2>(*it);
        auto phone_number = std::get<3>(*it);
        
        try
        {
            p_connection->prepare("insert_Client", 
                                  "INSERT INTO Client(first_name, last_name, email) "
                                  "VALUES ($1, $2, $3) ON CONFLICT (email) DO NOTHING");
            pqxx::work cl_transaction{ *p_connection };
            cl_transaction.exec_prepared("insert_Client", first_name, last_name, email);
            cl_transaction.commit();
            log_event("Client " + first_name + " " + last_name + " with email " + email + " added");
        }
        catch (const std::exception& ex)
        {
            insert_exception_structure ies;
            ies.where = it;
            ies.what = ex.what();
            log_event("Cannot add " + email);
            throw ies;
        }
        
        if (phone_number != 0)
        {
            try
            {
                add_phone_number(email, phone_number);
            }
            catch (const empty_value_exception& ev) {}
            catch (const std::exception& ex)
            {
                insert_exception_structure ies;
                ies.where = it;
                ies.what = ex.what();
                throw ies;
            }
        }
    }
}

void db_manager::add_phone_number(std::string email, unsigned long long phone_number)
{
    if (phone_number == 0)
    {
        throw empty_value_exception("Empty phone number");
    }
    
    p_connection->prepare("insert_PhoneNumber", 
                          "INSERT INTO PhoneNumber(ph_number, email) "
                          "VALUES ($1, $2) ON CONFLICT (email) DO NOTHING");
    pqxx::work ph_transaction{ *p_connection };
    ph_transaction.exec_prepared("insert_PhoneNumber", phone_number, email);
    ph_transaction.commit();
    log_event("Phone number " + std::to_string(phone_number) + " belonging to client with email " + email + " added");
}

void db_manager::modify_client(std::string email_before_changing, attribute::attribute attribute_to_change, std::string new_value)
{
    if (attribute_to_change == attribute::phone_number)
    {
        throw std::runtime_error("If you want to change a phone number, please, "
                                 "remove irrelevant one and add new value using the options of deleting and adding phone numbers.");
    }
    
    std::string update_query = "UPDATE Client SET ";
    std::string updated_attribute;
    switch (attribute_to_change)
    {
    case attribute::first_name:
        update_query += "first_name";
        updated_attribute = "first name";
        break;
    case attribute::last_name:
        update_query += "last_name";
        updated_attribute = "last name";
        break;
    case attribute::email:
        update_query += "email";
        updated_attribute = "email";
        break;
    default:
        break;
    }
    update_query += " = ($1) WHERE email = '";
    update_query += email_before_changing;
    update_query += "';";
    
    p_connection->prepare("update_Сlient", update_query);
    pqxx::work transaction{ *p_connection };
    transaction.exec_prepared("update_Сlient", new_value);
    transaction.commit();
    log_event("Client with email " + email_before_changing + " updated: new " + updated_attribute + " = '" + new_value + "'");
}

void db_manager::delete_phone_number(unsigned long long phone_number)
{
    std::string delete_query = "DELETE FROM PhoneNumber "
                               "WHERE ph_number = ($1);";
    p_connection->prepare("delete_PhoneNumber", delete_query);
    pqxx::work transaction{ *p_connection };
    transaction.exec_prepared("delete_PhoneNumber", std::to_string(phone_number));
    transaction.commit();
    log_event("Phone number " + std::to_string(phone_number) + " deleted");
}

void db_manager::delete_client(std::string email)
{
    clients_package client_to_delete = find_client(attribute::email, email);
    
    for (auto [first_name, last_name, email, phone_number] : client_to_delete.package)
    {
        delete_phone_number(phone_number);
    }
    
    std::string delete_query = "DELETE FROM Client "
                               "WHERE email = ($1);";
    p_connection->prepare("delete_Client", delete_query);
    pqxx::work transaction{ *p_connection };
    transaction.exec_prepared("delete_Client", email);
    transaction.commit();
    log_event("Client with email " + email + " deleted");
}

clients_package db_manager::find_client(attribute::attribute search_criterion, std::string value)
{
    std::string query = "SELECT Client.first_name, Client.last_name, Client.email, PhoneNumber.phone_number "
                        "FROM Client "
                        "LEFT JOIN PhoneNumber ON Client.email = PhoneNumber.email ";
    std::string where_statement = "WHERE ";
    std::string str_attribute;
    switch (search_criterion)
    {
    case attribute::first_name:
        where_statement += "Client.first_name = '";
        str_attribute = "first name";
        break;
    case attribute::last_name:
        where_statement += "Client.last_name = '";
        str_attribute = "last name";
        break;
    case attribute::email:
        where_statement += "Client.email = '";
        str_attribute = "email";
        break;
    case attribute::phone_number:
        where_statement += "PhoneNumber.phone_number = '";
        str_attribute = "phone number";
        break;
    }
    where_statement += value;
    where_statement += "';";
    query += where_statement;
    
    log_event("Searching clients with " + str_attribute + " = '" + value + "'...");
    
    pqxx::work transaction{ *p_connection };
    
    clients_package query_result;
    for (auto row : transaction.query<std::string, std::string, std::string, unsigned long long>(query))
    {
        query_result.package.push_back(row);
    }
    
    return query_result;
}
