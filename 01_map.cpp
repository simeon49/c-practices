#include <stdio.h>
#include <string>
#include <map>
using namespace std;



/**
	map 类似与python中的字典 在stl中是使用 RBtree 实现
	map的基本操作函数：
		C++ Maps是一种关联式容器，包含“关键字/值”对
		begin()          返回指向map头部的迭代器
		clear(）         删除所有元素
		count()          返回指定元素出现的次数
		empty()          如果map为空则返回true
		end()            返回指向map末尾的迭代器
		equal_range()    返回特殊条目的迭代器对
		erase()          删除一个元素
		find()           查找一个元素
		get_allocator()  返回map的配置器
		insert()         插入元素
		key_comp()       返回比较元素key的函数
		lower_bound()    返回键值>=给定元素的第一个位置
		max_size()       返回可以容纳的最大元素个数
		rbegin()         返回一个指向map尾部的逆向迭代器
		rend()           返回一个指向map头部的逆向迭代器
		size()           返回map中元素的个数
		swap()            交换两个map
		upper_bound()     返回键值>给定元素的第一个位置
		value_comp()      返回比较元素value的函数

	参考: 
	http://blog.sina.com.cn/s/blog_61533c9b0100fa7w.html
 */


void show_map(const map<string, int> mp)
{
	map<string, int>::const_iterator it;
	printf("{");
	for (it = mp.begin(); it != mp.end(); it ++) {
		printf("\"%s\":%d, ", it->first.c_str(), it->second);
	}
	printf("}\n");
}

int main(int argc, char const *argv[])
{
	map<string, int> map_0;
	map<string, int> map_1;

	printf("map_0 插入数据:\n");
	map_0.insert(pair<string, int>("Jim", 99));
	map_0["John"] = 80;
	map_0["Ken"] = 90;
	map_0["Li"] = 87;
	show_map(map_0);

	printf("map_1 插入数据:\n");
	map_1.insert(pair<string, int>("A", 1));
	map_1["B"] = 2;
	map_1["C"] = 3;
	map_1["D"] = 4;
	show_map(map_1);


	printf("交换:\n");
	//Map中的swap不是一个容器中的元素交换，而是两个容器交换；
	map_0.swap(map_1);
	show_map(map_0);
	show_map(map_1);

	printf("查找元素:\n");
	map<string, int>::iterator it;
	it = map_0.find("John");
	if (it != map_0.end()) {
		printf("Key=\"%s\"  Value=%d\n", it->first.c_str(), it->second);

		printf("删除元素:\n");
		map_0.erase(it);
		map_0.erase("John");
		show_map(map_0);
	}
	return 0;
}