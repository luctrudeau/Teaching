#include "Child.h"

int sub(struct Object o)
{
	return o.a - o.b;
}

struct Object buildChild(int a, int b)
{
	struct Object o = build(a,b);
	o.myFunction = &sub;
	return o;
}