#include <stdio.h>
template<typename T, unsigned int N>
struct MArray{
    public:
        MArray(T xx) : aa(xx){}
        unsigned int getSize(){
            return N;
        }
    private:
        T a[N];
        T aa;
};
template<unsigned int N=5>
struct Wrap{
    Wrap(MArray<int, N> x) : mArray(x){};
    MArray<int, N> mArray;
};

template<typename T, unsigned int N>
unsigned int array_size(T (&array)[N]){
    return N;
}
/*
template<typename T, unsigned int N>
unsigned int array_size(T array[N]){
    return N;
}
这种声明不ok，调用会报匹配不到对应模板
因为编译器将具有数组类型的参数调整为指向数组元素类型的指针，实际上类似：
template<typename T, unsigned int N>
unsigned int array_size(T* array){
    return N;
}
在array_size(x)调用时，参数数组x被隐式转换为指向其第一个元素的指针，
因此编译器无法推导出模板非类型参数N的值，
当参数声明为引用时，就不会发生上述的调整和隐式转换，引用仅用作数组的别名。
*/


int main(){
    int x[3] = {1, 2, 3};
    MArray<int, 3> mArray(x[0]);
    Wrap<3> wrap(mArray);
    printf("%d %d\n", mArray.getSize(), array_size(x));
}