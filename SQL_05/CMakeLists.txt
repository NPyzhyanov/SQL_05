cmake_minimum_required(VERSION 3.8)
project(SQL_05 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("../libpqxx-7.9.0" libpqxx-build)

set(HEADERS_LIST db_manager.h connection_info.h attribute.h clients_package.h insert_exception_structure.h empty_value_exception.h log_event.h add_clients_batch.h consts.h pick_attribute.h check_phone_number.h print_clients_package.h csv_parser.h)
set(SRC_LIST setup_connection.cpp db_manager.cpp log_event.cpp add_clients_batch.cpp pick_attribute.cpp check_phone_number.cpp print_clients_package.cpp)

add_executable(${PROJECT_NAME} main.cpp ${HEADERS_LIST} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE pqxx)
