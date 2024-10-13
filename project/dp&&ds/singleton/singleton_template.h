#pragma once
#include <iostream>

//抽象出单例模板
namespace single{

template <typename T>
class Singleton{
public:
    static T* instance(){
        //为保证线程安全
        //这里需要加锁和double check
        if(instance_ == NULL){
            instance_ = new T();
        }
        return instance_;
    }
private:
    Singleton(){};
    Singleton(const Singleton<T>&);
    ~Singleton();
    Singleton<T>& operator=(const Singleton<T>&);

private:
    static T* instance_;

};
//模板类的静态成员赋值写法
//是不是给每一个T类都会编译出一句这个赋值语句
template <typename T>
T* Singleton<T>::instance_ = NULL;

} // namespace single

