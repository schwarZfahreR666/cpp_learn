#include <stdio.h>

class Test{
    public:
        int a;
        int b;
};

Test& make(){
    Test a;
    a.a = 1;
    a.b = 2;
    return a;
}
//返回栈上变量的引用，当该位置内存被改写后，会出现错误
int& make1(){
    int i = 100;
    int& ref = i;
    return ref;
}

void cover(){
    char s[] = "gjgjhgjaaaaaaahhhhhhhhhhhhhaahhahahahhahahahahjhkhvvjygcvgvhvhvkhjvyycvgvgv";
}

int main(){
    int& x = make1();
    volatile Test m = make();
    // cover();
    printf("%d %d\n", x, x);
}