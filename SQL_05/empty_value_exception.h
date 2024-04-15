#ifndef EMPTY_VALUE_EXCEPTION_H
#define EMPTY_VALUE_EXCEPTION_H

class empty_value_exception : public std::exception
{
public:
    empty_value_exception(const std::string what_arg_) : what_arg(what_arg_) {}
    
    const char* what() const noexcept { return what_arg.c_str(); }
    
private:
    std::string what_arg;
};

#endif // EMPTY_VALUE_EXCEPTION_H
