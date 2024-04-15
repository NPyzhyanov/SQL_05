#include "check_phone_number.h"

bool check_phone_number(const unsigned long long &number)
{
    return ((number >= 79000000000 && number <= 79999999999) || number == 0);
}
