#ifndef DB_MANAGER_H
#define DB_MANAGER_H

class db_manager
{
public:
    db_manager();
    ~db_manager() = default;
    
    db_manager(const db_manager&) = delete;
    db_manager& operator=(const db_manager&) = delete;
    db_manager(db_manager&&) = delete;
    db_manager& operator=(db_manager&&) = delete;
    
    void setup_connection(); // described in setup_connection.cpp
    bool check_connection() { return connection_set_up; }
    
    void create_db();
    void add_client(clients_package &clients_to_add);
    void add_phone_number(std::string email, unsigned long long phone_number);
    void modify_client(std::string email, attribute::attribute attribute_to_change, std::string new_value);
    void delete_phone_number(unsigned long long phone_number);
    void delete_client(std::string email);
    clients_package find_client(attribute::attribute search_criterion, std::string value);
    
private:
    std::unique_ptr<pqxx::connection> p_connection;
    
    bool connection_set_up;
    bool db_is_empty;
    
    connection_info conn_info;
    
    std::string build_connection_argument(std::string password);
    bool check_number(const unsigned long long &number);
};

#endif // DB_MANAGER_H
