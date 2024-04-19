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
#include "log_event.h"
#include "db_manager.h"
#include "add_clients_batch.h"
#include "print_clients_package.h"

int main()
{
    db_manager dbm;
    
    if (!dbm.check_connection())
    {
        return 0;
    }
    
    // 0. Очистка БД
    dbm.clear_db();
    
    // 1. Создание структуры БД (таблицы).
    log_event("\nTrying to create database...");
    dbm.create_db();
    
    // 2. Добавление нового клиента
    log_event("\nTrying to add clients from file \"good_clients_batch.csv\"...");
    try
    {
        add_clients_batch(dbm, "../SQL_05/input_data/good_clients_batch.csv");
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    
    log_event("\nTrying to add clients from file \"wrong_clients_batch.csv\"...");
    try
    {
        add_clients_batch(dbm, "../SQL_05/input_data/wrong_clients_batch.csv");
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    /*
    // 3. Добавление телефона для существующего клиента.
    log_event("\nTrying to add phone number '79440005000' belonging to client with email \"galinaserova@yashchik.ru\"...");
    try
    {
        dbm.add_phone_number("galinaserova@yashchik.ru", 79440005000);
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    
    // 4. Изменение данных о клиенте.
    log_event("\nTrying to change client's email \"igoryan999@yashchik.ru\": new value = \"igoryan888@yashchik.ru\"...");
    try
    {
        dbm.modify_client("igoryan999@yashchik.ru", attribute::email, "igoryan888@yashchik.ru");
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    
    // 5. Удаление телефона у существующего клиента.
    log_event("\nTrying to delete phone number '79003008000' belonging to client with email \"lena_zh@yashchik.ru\"...");
    try
    {
        dbm.delete_phone_number(79003008000);
    }
    catch (const std::exception &ex)
    {
        std::cout << "Cannot delete this phone number. " << ex.what() << std::endl;
    }
    
    // 6. Удаление существующего клиента.
    log_event("\nTrying to delete client with email \"white-spirit@yashchik.ru\"...");
    try
    {
        dbm.delete_client("white-spirit@yashchik.ru");
    }
    catch (const std::exception &ex)
    {
        std::cout << "Cannot delete this Client. " << ex.what() << std::endl;
    }
    
    // 7. Поиск клиента по его данным — имени, фамилии, email или телефону.
    log_event("\nTrying to find clients with email \"igoryan999@yashchik.ru\"...");
    print_clients_package(dbm, attribute::email, "igoryan999@yashchik.ru");
    
    log_event("\nTrying to find clients with first name \"Елена\"...");
    print_clients_package(dbm, attribute::first_name, "Елена");
    
    log_event("\nTrying to find clients with phone number \"79000005555\"...");
    print_clients_package(dbm, attribute::phone_number, "79000005555");
    */
    return 0;
}
