#ifndef INSERT_EXCEPTION_STRUCTURE_H
#define INSERT_EXCEPTION_STRUCTURE_H

struct insert_exception_structure
{
    std::vector<std::tuple<std::string, std::string, std::string, unsigned long long>>::iterator where;
    std::string what;
};

#endif // INSERT_EXCEPTION_STRUCTURE_H
