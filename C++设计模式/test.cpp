#include <stdio.h>


class MethodBase
{
public:
	MethodBase(){}
	virtual ~MethodBase(){printf("~MethodBase\n");}
	
	void doit(void) {
		step1();
		step2();
		step3();
	}
protected:
	virtual void step1(void) = 0;
	virtual void step2(void) = 0;
	virtual void step3(void) = 0;
};

class Method1:public MethodBase
{
private:
	Method1(){}
public:
	~Method1(){
		printf("~Method1\n");
	}
	static MethodBase *getInstance(void) {
		return s_p_instance;
	}
protected:
	void step1(void) {printf("step1 in Method1\n");}
	void step2(void) {printf("step2 in Method1\n");}
	void step3(void) {printf("step3 in Method1\n");}
private:
	static MethodBase *s_p_instance;
};


class Method2:public MethodBase
{
private:
	Method2(){}
public:
	~Method2(){
		printf("~Method2\n");
	}
	static MethodBase *getInstance(void) {
		return s_p_instance;
	}
protected:
	void step1(void) {printf("step1 in Method2\n");}
	void step2(void) {printf("step2 in Method2\n");}
	void step3(void) {printf("step3 in Method2\n");}
private:
	static MethodBase *s_p_instance;
};


MethodBase *Method1::s_p_instance = new Method1();
MethodBase *Method2::s_p_instance = new Method2();

int main(int argc, char const *argv[])
{
	MethodBase *method;

	method = Method1::getInstance();
	method->doit();

	delete method;
	return 0;
}