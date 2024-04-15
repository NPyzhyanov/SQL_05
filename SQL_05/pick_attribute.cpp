#include <string>
#include <stdexcept>

#include "attribute.h"

attribute::attribute pick_attribute(std::string value)
{
    if (value == "first_name")
    {
        return attribute::first_name;
    }
    if (value == "last_name")
    {
        return attribute::last_name;
    }
    if (value == "email")
    {
        return attribute::email;
    }
    if (value == "phone_number")
    {
        return attribute::phone_number;
    }
    throw std::runtime_error("\"" + value + "\" is incorrect attribute name");
}
