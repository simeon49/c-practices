#include <stdio.h>

/**
 * 饮料（基类）
 * 制作饮料的步骤 
 * 1.烧开水 boilWater（）
 * 2.加入饮料
 * 3.将开水倒入杯中 pourWater（）
 * 4.放入调味品(可选) addCondiments（）
 */
class Beverage
{
public:
	/**
	 * 制作饮料
	 */
	void makeBeverage(void) {
		boilWater();
		pourBeverage();
		pourWater();
		if (isNeedAddCondiments()) {
			addCondiments();
		}
	}

protected:
	// 放饮料
	virtual void pourBeverage(void) = 0;
	// 放入调味品
	virtual void addCondiments(void) {};
	// 为true时添加调味品 否则不添加 （钩子函数）
	virtual bool isNeedAddCondiments(void) {
		return true;
	}

private:
	// 烧开水
	virtual void boilWater(void) {
		printf("\t烧开水\n");
	}
	// 将开水倒入杯中
	virtual void pourWater(void) {
		printf("\t将开水倒入杯中\n");
	}
};

/**
 * 咖啡
 */
class Coffee:public Beverage
{
protected:
	void pourBeverage(void) {
		printf("\t加入咖啡冲剂\n");
	}
	void addCondiments(void) {
		printf("\t加入糖\n");
	}
};

/**
 * 茶
 */
class Tea:public Beverage
{
protected:
	void pourBeverage(void) {
		printf("\t放入茶叶\n");
	}

	//我们不需要添加其它东西
	bool isNeedAddCondiments(void) {
		return false;
	}
};

int main(int argc, char const *argv[])
{
	Beverage *drink;

	printf("\n==== 制作 Coffee ====\n");
	drink = new Coffee;
	drink->makeBeverage();
	delete drink;

	printf("\n==== 制作 茶 ====\n");
	drink = new Tea;
	drink->makeBeverage();
	delete drink;
	return 0;
}