#ifndef TypeNames_H
#define TypeNames_H

#include <Arduino.h>

template<typename T>
struct TypeName {
    static const char* get();
};

template<> const char* TypeName<void>::get() { return "void";    }
template<> const char* TypeName<bool>::get() { return "boolean"; }
template<> const char* TypeName<float>::get() { return "decimal"; }
template<> const char* TypeName<double>::get() { return "decimal"; }
template<> const char* TypeName<char>::get() { return "integer"; }
template<> const char* TypeName<unsigned char>::get() { return "integer"; }
template<> const char* TypeName<signed int>::get() { return "integer"; }
template<> const char* TypeName<unsigned int>::get() { return "integer"; }
template<> const char* TypeName<signed short>::get() { return "integer"; }
template<> const char* TypeName<unsigned short>::get() { return "integer"; }
template<> const char* TypeName<signed long>::get() { return "integer"; }
template<> const char* TypeName<unsigned long>::get() { return "integer"; }
template<> const char* TypeName<const char*>::get() { return "string"; }
template<> const char* TypeName<char *>::get() { return "string"; }
template<> const char* TypeName<String>::get() { return "string"; }
template<> const char* TypeName<std::string>::get() { return "string"; }

#endif