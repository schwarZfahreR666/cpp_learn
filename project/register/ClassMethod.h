#pragma once

#include <string>
using namespace std;

namespace regist {

class ClassMethod
{
public:
    ClassMethod() : name_(""), method_(0) {}
    ClassMethod(const string & name, uintptr_t method) : name_(name), method_(method) {}
    ~ClassMethod() {}

    const string & name()
    {
        return name_;
    }

    uintptr_t method()
    {
        return method_;
    }

private:
    string name_;
    uintptr_t method_;
};

} // namespace regist