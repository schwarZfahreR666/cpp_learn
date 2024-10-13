//1.类的构造方法定义为私有(构造、拷贝构造、析构、拷贝赋值)
//2.定义一个私有的类的静态实例
//3.提供共有的获取类的方法

#include "singleton_template.h"
#include "A.h"
using namespace single;
int main(){
    Singleton<A>::instance()->show();
    return 0;
}