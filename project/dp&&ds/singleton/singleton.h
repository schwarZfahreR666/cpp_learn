#pragma once
#include <iostream>
#include <string>

class A{
private:
    A() : a_name("A"){};
    A(const A&);
    ~A();
    A& operator=(const A&);

private:
    static A* a_instance;
    std::string a_name;

public:
    static A* getAInstance(){
        if(a_instance == NULL){
            a_instance = new A();
        }
        return a_instance;
    }

    void show(){
        std::cout << a_name << std::endl;
    }

};

A* A::a_instance = NULL;