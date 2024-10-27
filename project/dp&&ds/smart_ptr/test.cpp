#include "shared_ptr.h"
#include <iostream>
using std::cout;
using std::endl;

struct MyStruct
{
	MyStruct() = default;
	MyStruct(int a, int b) :a(a), b(b) {}
	int a;
	int b;
};

int main()
{
	MyStruct *s = new MyStruct();
	s->a = 10;
	s->b = 20;

	smart_ptr::shared_ptr<MyStruct> sp(s);

	cout << sp->a << endl;
	cout << sp->b << endl;
	cout << (*sp).a << endl;

	auto sp2 = smart_ptr::make_shared<MyStruct>(100, 200);
	cout << sp2->a << endl;
	cout << sp2->b << endl;

    auto sp3 = sp2;
    sp2->a++;
    cout << sp3->a << endl;
	cout << sp3->b << endl;


	auto p = sp2.release();
	cout << p->a << endl;
	cout << p->b << endl;
    
    sp3.release();
	delete p;

	return 0;
}