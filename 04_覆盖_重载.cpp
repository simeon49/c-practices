#include <stdio.h>
/**
 *  Test 1
 *  	重载:	（1）相同的范围（在同一个类中）；
				（2）函数名字相同；
				（3）参数不同；
				（4）virtual 关键字可有可无。
 
 *  	覆盖:	（1）不同的范围（分别位于派生类与基类）；
				（2）函数名字相同；
				（3）参数相同；
				（4）基类函数必须有virtual 关键字。

 *  	隐藏:	（1）如果派生类的函数与基类的函数同名，但是参数不同。此时，不论有无virtual
					关键字，基类的函数将被隐藏（注意别与重载混淆）。
				（2）如果派生类的函数与基类的函数同名，并且参数也相同，但是基类函数没有virtual
					关键字。此时，基类的函数被隐藏（注意别与覆盖混淆）。
 */
class Base {
public:
	Base() {printf("Base init.\n");}
	virtual ~ Base() {printf("Base release.\n");}	
	virtual int foo() {printf("Base foo()\n");}			
	int foo(int num) {printf("Base foo(%d)\n", num);}	// foo(int) 是foo()的重载
};

class ChildA: virtual public Base {
public:
	ChildA() {printf("ChildA init.\n");}
	~ ChildA() {printf("ChildA release.\n");}
	int foo() {printf("ChildA foo()\n");}		// foo() 覆盖可 Base::foo() 方法, 同时隐藏了 Base::foo(int)方法
};

class ChildB: virtual public Base {
public:
	ChildB() {printf("ChildB init.\n");}
	~ ChildB() {printf("ChildB release.\n");}
	int foo() {printf("ChildB foo()\n");}
};

class ChildC: public ChildA, public ChildB {
public:
	ChildC() {printf("ChildC init.\n");}
	~ ChildC() {printf("ChildC release.\n");}
	int foo() {printf("ChildC foo()\n");}
};


int main(int argc, char const *argv[])
{
	ChildA a;
	Base *p_base = &a;
	ChildA *p_child_a = &a;
	printf("=================================\n");
	p_base->foo();
	p_base->foo(1);

	p_child_a->foo();
	// p_child_a->foo(1);	//  ChildA 中的 foo() 方法隐藏了 Base类中foo(int num)方法  如果需要使用foo(int) 需要在ChildA中实现foo(int)方法
	// fun_0(*p_base);
	// fun_1(*p_base);
	// fun_2(p_base);
	printf("=================================\n");
	delete p_base;
	return 0;
}

 