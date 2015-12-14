#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdio.h>

struct Object;

struct Object build(int a, int b);

struct Object {
	int a;
	int b;
	int (*myFunction)(struct Object);
	void (*print)(struct Object); 
};

#endif