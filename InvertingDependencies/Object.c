#include "Object.h"

int add(struct Object o)
{
	return o.a + o.b;
}

void print(struct Object o)
{
	printf("Result = %d\n", o.myFunction(o));
}

struct Object build(int a, int b)
{
	struct Object o;
	o.a = a;
	o.b = b;
	o.myFunction = &add;
	o.print = &print;
	return o;
}