#include <stdio.h> 
#include <algorithm>
using namespace std;

/**
 * http://blog.csdn.net/xfortius/article/details/7760490
 */

// typedef struct rect {
// 	int length;
// 	int width;
// 	struct rect & operator= (const struct rect &r0) {
// 		printf("===\n");
// 		this->length = r0.length;
// 		this->width = r0.width;
// 		return *this;
// 	}
// } Rect;

class Rect
{
public:
	Rect(int length, int width) {
		m_length = length;
		m_width = width;
	}
	~Rect() {};
	Rect(const Rect &r0) {
		// printf("Rect copy construct\n");
		m_length = r0.m_length;
		m_width = r0.m_width;
	}

	int getLength() const {
		return m_length;
	}

	int getWidth() const {
		return m_width;
	}

	void setSize(int length, int width) {
		m_length = length;
		m_width = width;
	}

	Rect & operator = (Rect &r0) {
		// printf("Operator = \n");
		m_length = r0.m_length;
		m_width = r0.m_width;
		return *this;
	}

	bool operator < (const Rect &r0) const {
		return (m_length*m_width) < (r0.m_length*r0.m_width);
	}
private:
	int m_length;
	int m_width;
};

/**
  * Vector:
	(1) 指定一块如同数组一样的连续存储，但空间可以动态扩展。即它可以像数组一样操作，并且可以进行动态操作。通常体现在push_back() 
		pop_back() 。
	(2) 随机访问方便，它像数组一样被访问，即支持[ ] 操作符和vector.at()
	(3) 节省空间，因为它是连续存储，在存储数据的区域都是没有被浪费的，但是要明确一点vector 大多情况下并不是满存的，在未存
		储的区域实际是浪费的。
	(4) 在内部进行插入、删除操作效率非常低，这样的操作基本上是被禁止的。Vector 被设计成只能在后端进行追加和删除操作，其原
		因是vector 内部的实现是按照顺序表的原理。
	(5) 只能在vector 的最后进行push 和pop ，不能在vector 的头进行push 和pop 。
	(6) 当动态添加的数据超过vector 默认分配的大小时要进行内存的重新分配、拷贝与释放，这个操作非常消耗性能。 所以要vector 
		达到最优的性能，最好在创建vector 时就指定其空间大小。
  */
#include <vector>
void show_rect_vec(const std::vector<Rect> &rect_vec)
{
	// 是用下标的方式访问
	// for (int i = 0; i < rect_vec.size(); i ++) {
	// 	printf("(%d,%d) ", rect_vec[i].getLength(), rect_vec[i].getWidth());
	// }

	// 使用迭代器访问元素
	vector<Rect>::const_iterator it;
	for (it = rect_vec.begin(); it != rect_vec.end(); it ++) {
		printf("(%d,%d) ", it->getLength(), it->getWidth());
	}
	printf("\n  total: %d\n\n", rect_vec.size());
}

void vector_test(void)
{
	printf("创建一个 Vector 包含3个{0, 0}:\n");
	Rect rect(0, 0);
	std::vector<Rect> vec(3, rect);
	show_rect_vec(vec);
	
	printf("尾部插入数字:\n");
	rect.setSize(7, 3);
	vec.push_back(rect);
	show_rect_vec(vec);

	printf("Pop 一个元素:\n");
	vec.pop_back();
	show_rect_vec(vec);

	printf("Insert 一个元素(在第一个元素前):\n");
	rect.setSize(9, 6);
	vec.insert(vec.begin(), rect);
	show_rect_vec(vec);

	printf("Sort 排序:\n");
	sort(vec.begin(), vec.end());
	show_rect_vec(vec);

	printf("Erase 一个元素(删除第2到最后一个元素):\n");
	vec.erase(vec.begin()+1, vec.end());
	show_rect_vec(vec);

	printf("清空 Vector:\n");
	vec.clear();
	show_rect_vec(vec);
}



/**
 * 双向链表list
	是一个线性链表结构，它的数据由若干个节点构成，每一个节点都包括一个信息块（即实际存储的数据）、一个前驱指针和一个后驱指针。
	它无需分配指定的内存大小且可以任意伸缩，这是因为它存储在非连续的内存空间中，并且由指针将有序的元素链接起来。
	由于其结构的原因，list 随机检索的性能非常的不好，因为它不像vector 那样直接找到元素的地址，而是要从头一个一个的顺序查找，
	这样目标元素越靠后，它的检索时间就越长。检索时间与目标元素的位置成正比。
	虽然随机检索的速度不够快，但是它可以迅速地在任何节点进行插入和删除操作。因为list 的每个节点保存着它在链表中的位置，插入
	或删除一个元素仅对最多三个元素有所影响，不像vector 会对操作点之后的所有元素的存储地址都有所影响，这一点是vector 不可比拟的。

	list 的特点：
	(1) 不使用连续的内存空间这样可以随意地进行动态操作；
	(2) 可以在内部任何位置快速地插入或删除，当然也可以在两端进行push 和pop 。
	(3) 不能进行内部的随机访问，即不支持[ ] 操作符和vector.at() ；
	(4) 相对于verctor 占用更多的内存。
 */
#include <list>
void show_rect_list(const list<Rect> rect_lst)
{
	list<Rect>::const_iterator it;
	for (it = rect_lst.begin(); it != rect_lst.end(); it ++) {
		printf("(%d,%d) ", it->getLength(), it->getWidth());
	}
	printf("\n  total: %d\n\n", rect_lst.size());
}
void list_test(void)
{
	printf("创建一个 List:\n");
	Rect rect(0, 0);
	list<Rect> lst;
	show_rect_list(lst);
	
	printf("添加元素:\n");
	rect.setSize(3, 3);
	lst.push_front(rect);
	rect.setSize(2, 3);
	lst.push_front(rect);
	rect.setSize(1, 3);
	lst.push_front(rect);
	rect.setSize(5, 3);
	lst.push_back(rect);
	rect.setSize(4, 5);
	// 在第4个位置插入元素
	list<Rect>::iterator it = lst.begin();
	for (int i = 0; i < 4 && it != lst.end(); it ++) {
		i ++;
	}
	lst.insert(--it, 1, rect);
	show_rect_list(lst);

	printf("合并list((11, 0), (12,0)):\n");
	list<Rect> lst_1;
	rect.setSize(11, 0);
	lst_1.push_back(rect);
	rect.setSize(12, 0);
	lst_1.push_back(rect);
	lst.splice(lst.end(), lst_1);
	show_rect_list(lst);
	// show_rect_list(lst_1);	// lst_1 为空

	printf("删除元素:\n");
	// 删除第4个位置的元素
	it = lst.begin();
	for (int i = 0; i < 4 && it != lst.end(); it ++) {
		i ++;
	}
	lst.erase(--it);
	lst.pop_front();
	lst.pop_back();
	show_rect_list(lst);

	printf("Sort 排序:\n");
	lst.sort();
	show_rect_list(lst);

	printf("清空 Lis:\n");
	lst.clear();
	show_rect_list(lst);
}


/**
 * 双端队列deque 
	是一种优化了的、对序列两端元素进行添加和删除操作的基本序列容器。它允许较为快速地随机访问，
	但它不像vector 把所有的对象保存在一块连续的内存块，而是采用多个连续的存储块，并且在一个映
	射结构中保存对这些块及其顺序的跟踪。向deque 两端添加或删除元素的开销很小。它不需要重新分
	配空间，所以向末端增加元素比vector 更有效。
	实际上，deque 是对vector 和list 优缺点的结合，它是处于两者之间的一种容器。
	deque 的特点：
	(1) 随机访问方便，即支持[ ] 操作符和vector.at() ，但性能没有vector 好；
	(2) 可以在内部进行插入和删除操作，但性能不及list ；
	(3) 可以在两端进行push 、pop ；
 */
#include <deque>
void show_rect_deque(const deque<Rect> rect_deque)
{
	deque<Rect>::const_iterator it;
	for (it = rect_deque.begin(); it != rect_deque.end(); it ++) {
		printf("(%d,%d) ", it->getLength(), it->getWidth());
	}
	printf("\n  total: %d\n\n", rect_deque.size());
}

void deque_test(void)
{
	printf("创建一个 Deque:\n");
	Rect rect(0, 0);
	deque<Rect> deque;
	show_rect_deque(deque);

	
	printf("添加元素:\n");
	rect.setSize(3, 3);
	deque.push_front(rect);
	rect.setSize(2, 3);
	deque.push_front(rect);
	rect.setSize(1, 3);
	deque.push_front(rect);
	rect.setSize(5, 3);
	deque.push_back(rect);
	rect.setSize(4, 5);
	show_rect_deque(deque);

	printf("删除元素:\n");
	// 删除第4个位置的元素
	deque.pop_front();
	deque.pop_back();
	show_rect_deque(deque);

	printf("Sort 排序:\n");
	
	show_rect_deque(deque);

	printf("清空 Deque:\n");
	deque.clear();
	show_rect_deque(deque);
}


int main(int argc, char const *argv[])
{
	vector_test();	// Vector
	// list_test();		// List
	// deque_test();		// Deque
	return 0;
}