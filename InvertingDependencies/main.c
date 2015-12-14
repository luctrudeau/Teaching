#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Object.h"
#include "Child.h"

int main()
{
	struct Object o;
	
	srand(time(0));

	if (rand() % 2)
	{
		o = buildChild(5, 3);
	} else {
		o = build(2, 3);
	}

	o.print(o);
}
