#pragma once

#include <string>
using namespace std;

namespace regist{
class ClassField
{
public:
    ClassField() : name_(""), type_(""), offset_(0) {}
    ClassField(const string & name, const string & type, size_t offset) : name_(name), type_(type), offset_(offset) {}
    ~ClassField() {}

    const string & name()
    {
        return name_;
    }

    const string & type()
    {
        return type_;
    }

    size_t offset()
    {
        return offset_;
    }

private:
    string name_;
    string type_;
    size_t offset_;
};

} // namespace regist