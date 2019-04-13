#include <stdio.h>

class Singleton;
class Singleton
{
private:
	Singleton(){}
public:
	~Singleton() {
		if (-- m_g_total == 0) {
			printf("disconstructed.\n");
			delete m_g_instance;
		}
	}
	static Singleton &getInstance(void) {
		if (m_g_total == 0) {
			printf("constructed.\n");
			m_g_instance = new Singleton();
		}
		m_g_total ++;
		return *m_g_instance;
	}
	void show(void){
		printf("addr = 0x%x\n", this);
	}
private:
	static Singleton *m_g_instance;
	static int m_g_total;
};

Singleton *Singleton::m_g_instance = NULL;
int Singleton::m_g_total = 0;

int main(int argc, char const *argv[])
{
	Singleton s1 = Singleton::getInstance();
	Singleton s2 = Singleton::getInstance();
	s1.show();
	s2.show();
	return 0;
}