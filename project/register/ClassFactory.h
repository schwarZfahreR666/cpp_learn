#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
using namespace std;

#include "../dp&&ds/singleton/singleton_template.h"
using namespace single;

#include "ClassField.h"
#include "ClassMethod.h"

namespace regist {
//类继承Object，把field在object的offset注册到register中。
//当获取的时候，先从register获取offset
//再根据offset从object指针偏移取filed值

//把新建类的函数指针也注册到register中
//把类method的指针也注册到register中
class Object
{
public:
    Object();
    virtual ~Object();

    void set_class_name(const string & className);
    const string & get_class_name() const;

    int get_field_count();
    ClassField * get_field(int pos);
    ClassField * get_field(const string & fieldName);

    template <typename T>
    void get(const string & fieldName, T & value);

    template <typename T>
    void set(const string & fieldName, const T & value);
    void set(const string & fieldName, const char * value);
    
    void call(const string & methodName);
    virtual void show() = 0;

private:
    string m_className;
};

typedef Object * (*create_object)(void);

class ClassFactory
{
    friend class Singleton<ClassFactory>;
public:
    // reflect class
    void register_class(const string & className, create_object method);
    Object * create_class(const string & className);

    // reflect class field
    void register_class_field(const string & className, const string & fieldName, const string & fieldType, size_t offset);
    int get_class_field_count(const string & className);
    ClassField * get_class_field(const string & className, int pos);
    ClassField * get_class_field(const string & className, const string & fieldName);

    // reflect class method
    void register_class_method(const string & className, const string &methodName, uintptr_t method);
    int get_class_method_count(const string & className);
    ClassMethod * get_class_method(const string & className, int pos);
    ClassMethod * get_class_method(const string & className, const string & methodName);

private:
    ClassFactory() {}
    ~ClassFactory() {}

private:
    std::map<string, create_object> m_classMap;
    std::map<string, std::vector<ClassField *> > m_classFields;
    std::map<string, std::vector<ClassMethod *> > m_classMethods;
};

template <typename T>
void Object::get(const string & fieldName, T & value)
{
    ClassField * field = Singleton<ClassFactory>::instance()->get_class_field(m_className, fieldName);
    size_t offset = field->offset();
    value = *((T *)((unsigned char *)(this) + offset));
}

template <typename T>
void Object::set(const string & fieldName, const T & value)
{
    ClassField * field = Singleton<ClassFactory>::instance()->get_class_field(m_className, fieldName);
    size_t offset = field->offset();
    *((T *)((unsigned char *)(this) + offset)) = value;
}

} // namespace regist