#ifndef CLIENTS_PACKAGE_H
#define CLIENTS_PACKAGE_H

struct clients_package
{
    std::vector<std::tuple<std::string, std::string, std::string, unsigned long long>> package;
};

#endif // CLIENTS_PACKAGE_H
