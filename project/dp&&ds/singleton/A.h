#pragma once
#include <iostream>
#include <string>
#include "singleton_template.h"
using namespace single;
class A{
    //为了防止A被实例化，构造函数仍要为私有
    //声明Singleton为该类友元
    //注意Singleton要特化
    friend class Singleton<A>;
private:
    A() : a_name("A"){};
    A(const A&);
    ~A();
    A& operator=(const A&);

private:
    std::string a_name;

public:

    void show(){
        std::cout << a_name << std::endl;
    }

};