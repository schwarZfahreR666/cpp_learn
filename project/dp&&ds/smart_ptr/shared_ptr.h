#pragma once

#include <functional>

namespace smart_ptr{
// 模仿shared_ptr实现一个智能指针
template <typename T>
class shared_ptr
{
public:
	shared_ptr();
	//这里不允许直接从内部变量赋值
	explicit shared_ptr(T*);
	shared_ptr(const shared_ptr&);
	shared_ptr(T*, std::function<void(T*)>);
	shared_ptr& operator=(const shared_ptr&);
	//重写解引用和取值方法
	T& operator*() const;
	T* operator->() const;

	//析构函数，智能指针最重要的特性
	~shared_ptr();
	// 向bool的类型转换
	explicit operator bool() const;

	bool unique();
	void reset();
	void reset(T*);
	void reset(T*, std::function<void(T*)>);
	T* release();

	T* get() const;

private:
	// 默认的deleter
	static std::function<void(T*)> default_del;

private:
	unsigned* m_p_use_count = nullptr; //引用计数
	T* m_pobject = nullptr;  //实际指针
	std::function<void(T*)> m_del = default_del;  //deleter
};

//默认的deleter是delete调用析构，然后设置内部指针为nullptr
template <typename T>
std::function<void(T*)> shared_ptr<T>::default_del = [](T*p) {delete p; p = nullptr; };


//可变模板参数，作为初始化内部对象的参数
//万能引用作为形参，一定要做完美转发！！！
template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args)
{
	shared_ptr<T> sp(new T(std::forward<Args>(args)...));
	return sp;
}


template <typename T>
shared_ptr<T>::shared_ptr()
	:m_pobject(nullptr), m_p_use_count(new unsigned(1))
{
}


template <typename T>
shared_ptr<T>::shared_ptr(T *p)
	:m_pobject(p), m_p_use_count(new unsigned(1))
{
}


template <typename T>
shared_ptr<T>::shared_ptr(T *p, std::function<void(T*)> del)
	:m_pobject(p), m_p_use_count(new unsigned(1)), m_del(del)
{
}

//拷贝构造函数，要给引用计数加1（并且是所有shared_ptr的引用计数）
template <typename T>
shared_ptr<T>::shared_ptr(const shared_ptr& rhs)
	:m_pobject(rhs.m_pobject), m_p_use_count(rhs.m_p_use_count), m_del(rhs.m_del)
{
	(*m_p_use_count)++;
}

//拷贝赋值函数，用参数的内部指针把本指针内部指针置换出来(引用计数做相应的增加和减少)
template <typename T>
shared_ptr<T>& shared_ptr<T>::operator =(const shared_ptr &rhs)
{
	// 使用rhs的deleter
	m_del = rhs.m_del;
	// 递增右侧运算对象的引用计数
	++(*rhs.m_p_use_count);
	// 递减本对象的引用计数
	if (--(*m_p_use_count) == 0)
	{
		// 如果管理的对象没有其他用户了，则释放对象分配的成员
		m_del(m_pobject);
		
		delete m_p_use_count;
	}

	m_p_use_count = rhs.m_p_use_count;
	m_pobject = rhs.m_pobject;

	return *this; // 返回本对象
}

//解引用直接返回内部指针的解引用
template <typename T>
T& shared_ptr<T>::operator*() const
{
	return *m_pobject;
}

//->优先级高于&
//->运算符重载写法
//对于形如x->y的形式，x必须是指针或者重载了operator->()的类对象
//1.如果x是指针，那么等价于(*x).y
//2.如果x是类对象，那么等价于(m = x.operator->()).y。如果m是指针，那么和上述类似；
//  如果m还含有operator->()的重载，那么将重复调用这个过程
template <typename T>
T* shared_ptr<T>::operator->() const
{
	return &this->operator*();
}

//计算引用计数，不足则析构
template <typename T>
shared_ptr<T>::~shared_ptr()
{
	if (--(*m_p_use_count) == 0)
	{
		m_del(m_pobject);
		m_pobject = nullptr;

		delete m_p_use_count;
		m_p_use_count = nullptr;
	}
}

//独占内部指针，即引用计数设为1
template <typename T>
bool shared_ptr<T>::unique()
{
	return *m_p_use_count == 1;
}

//重置本智能指针
template <typename T>
void shared_ptr<T>::reset()
{
	(*m_p_use_count)--;

	if (*m_p_use_count == 0)
	{
		m_del(m_pobject);
	}

	m_pobject = nullptr;
	*m_p_use_count = 1;
	m_del = default_del;
}

//重置本智能指针，用p替换内部指针
template <typename T>
void shared_ptr<T>::reset(T* p)
{
	(*m_p_use_count)--;

	if (*m_p_use_count == 0)
	{
		m_del(m_pobject);
	}

	m_pobject = p;
	*m_p_use_count = 1;
	m_del = default_del;
}

//重置智能指针，并设置deleter
template <typename T>
void shared_ptr<T>::reset(T *p, std::function<void(T*)> del)
{
	reset(p);
	m_del = del;
}

//释放内部指针，并返回
//若引用计数到0，也不delete内部指针
template <typename T>
T* shared_ptr<T>::release()
{
	(*m_p_use_count)--;

	//这里重置为1
	if (*m_p_use_count == 0)
	{
		*m_p_use_count = 1;
	}

	auto p = m_pobject;
	m_pobject = nullptr;

	return p;
}


template <typename T>
T* shared_ptr<T>::get() const
{
	return m_pobject;
}


template <typename T>
shared_ptr<T>::operator bool() const
{
	return m_pobject != nullptr;
}

} // namespace smart_ptr