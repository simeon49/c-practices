#include <stdio.h>
template <class T>
T max(T a, T b)
{
	return (a > b)?a:b;
}

int main(int argc, char const *argv[])
{
	printf("max(1, 2)=%d\n", max(1, 2));
	return 0;
}