#ifndef _B_H_
#define _B_H_
class A;	// 这里的A 是一个不完全类型 (编译器还不知道A的成员 更不知道它占用空间的大小)
class B
{
public:
private:
	A *m_a;
	// A m_a;	// 这样的定义需要知道A的大小
};
#endif