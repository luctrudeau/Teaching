InvertingDependencies
=====================
![](https://github.com/luctrudeau/InvertingDependencies/raw/master/logo.jpg)
*photo credit: [Stéfan](http://www.flickr.com/photos/st3f4n/4085958000/)<a href=""></a> via [photopin](http://photopin.com) [cc](http://creativecommons.org/licenses/by-nc-sa/2.0/)*

Polymorphism is the super power of the OO designer. However, many designers don't exploit this power, they use inheritance for structural purposes. In a language like Java, this results in [an abuse of the instanceof statement](http://www.javapractices.com/topic/TopicAction.do?Id=31). 

Maybe Polymorphism is not the right word; maybe we should use multiple personality disorder (not as sexy as polymorphism when you're trying to sell your OO suite...). Depending on the runtime context, the object does not change form, it does not morph, it changes behavior, it behaves as another object.

But the interesting part of this mechanism is how the dependencies align. Looking at the dependencies from a UML Class diagram viewpoint, inheritance goes in the opposite direction as composition or aggregation. 

The next example shows two different ways for the *print* method of **Object** to invoke the *myFunction* method of **Child**. The first case, **Child** inherites from **Object** ([Template Method Pattern](http://sourcemaking.com/design_patterns/template_method)) this require the *myFunction* method to be present in **Object**. In the second case, **Object** depends on **Child** and invokes it. 

![](http://www.plantuml.com/plantuml/svg/VT3D2i8m303WUvyYp8iY6smFC0Q2LvyXQmTMktQqgJBctjrCDnGSzj13yf6_AX1wYgqH8borxT1Z2u5wq_3ljNodfC4G83dLbqPI0XZ0vKlmSDP6lM8oNvN5YYmMwN8O1F3Rmyrf1PtNbhOxCIv0Jc0Md8AsFqOhIJlxhLmnbzd0W3LFZfB67_wtpwV2MXkUCrFkRhDoMfDn6hlqKKxRYQgnYi_q1000)

However looking at it from a UML sequence diagram viewpoint, the direction of the method invocation is not reversed.

![](http://www.plantuml.com/plantuml/svg/SoWkIImgAStDuUBAo4n9LKZBpoz9og_WuYe02cScPnOavfKeAELR-ITbfIQNA2aa5Yj0cAWAagsSCp9pKi3oO1ONLxIk7LIfWfL2GL5cNec69XTK02W5LWMKoQrqAyrBBidCpmDASpcavgK07GC0)

Concretely, inheritance is a tool that offers the possibility to inverse dependencies arrows while preserving the direction of behavioral arrows! This super power can be extremely powerful in light of architectural styles that impose constraints on the direction of dependencies.

Examples of constraints imposed on the direction of dependencies are Robert C. Martin's ["The Dependency Rule"](http://blog.8thlight.com/uncle-bob/2012/08/13/the-clean-architecture.html) and the layers architectural style. The later imposes a downward "can use" relationship between layers. By using inheritance, one can respect this structural constraint of downward dependencies yet allow behavior to invoke upwards.

Let see how all this works, but to do so, let's use a programming language that is not object oriented. This way the details will not be hidden away by syntactic sugar or by compiler magic.

Let's start by defining **Object** with a simple struct ([Object.h](https://github.com/luctrudeau/InvertingDependencies/blob/master/Object.h))

```C   
struct Object;

struct Object build(int a, int b);

struct Object {
    int a;
    int b;
    int (*myFunction)(struct Object);
    void (*print)(struct Object); 
};
```

The first line is a forward declaration, it's required because the **Object** structure is used inside the definition of the **Object** structure. Next, there's the **build** function that is the equivalent of a constructor/factory. Finally the **Object** structure is defined. Notice that the last two declarations are function pointers. These two function pointers are *myFunction* that expects an **Object** structure as parameter and *print* that also expects an **Object** structure as parameter.

Notice that all the implementation details are hidden in Object.c (this is encapsulation). The idea behind this header file is to expose only the structure but not the behavior. No one knows, how _build_ work, or even what's in _myFunction_ or _print_, but they know that these functions exist. This separation is key in flipping the direction of the dependency without flipping the direction of invocation.

To find out what the behavior is, we can look at the [Object.c](https://github.com/luctrudeau/InvertingDependencies/blob/master/Object.c) file.
```c
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
```

Now we know that _myFunction_ points to an add function, and that _print_ simply does a printf.

Even if not all languages use the same keyword, the concept of an abstract method is generally available. We can match this concept to the functions pointers. If these pointers are not assigned then the method is abstract. This is why you can't instantiate classes with abstract methods.

Notice that the parameter of the functions inside the **Object** structure are structures of type **Object**. For now, we can think of this as a solution to the problem of accessing the values of the instance of the **Object** structure, but we will see shortly that there's another more profound mechanism at work here. Some programming languages hide this detail behind the "this" keyword, others like python will make the instance parameter visible to developer and define it as the first parameter of every method.

Let's say main.c wants to use the **Object** structure, it only need to include "Object.h", it does not need to care about Object.c. If we look at the dependencies we can see the inversion effect. The main.c file depends on Object.h but Object.h does not depend upon Object.c, it's the other way around Object.c depends upon Object.h. The header file is an abstraction.

![](http://www.plantuml.com/plantuml/svg/XL1R3i8W4FpVKspwrIQM0vYcwGqy0mKIq08D3nFJzEvIC3LgL3xOPMPsTfV11-f2r1E0loWpKqSr-d2RHBgTOeMIYWlKGe_2UG0xdWKBM6cg364LKezRkkEd78dCZ9NlUAcmR3Uo4XV0T5B7cDv5zudW6DN4QsK2qWDkOMnWVTgmXqqMS5w_5jjc8OIiMN6rYkFial27w6zlBDeqmRO-jlssoH6hhbE6JP4Bt_SLfB7QE7yX0GPXUFhZEm00)

Now let's try inheritance, I'm not interested in structural inheritance, I don't want to add a third variable to **Object**. I'm interested in polymorphism; I want to change the behavior of **Object**.

This is the content of [Child.h](https://github.com/luctrudeau/InvertingDependencies/blob/master/Child.h)
```c
#include "Object.h"

struct Object buildChild(int a, int b);
```

The **Child** depends on Object.h and defines a new constructor/factory *buildChild*. Let's look at its behavior ([Child.c](https://github.com/luctrudeau/InvertingDependencies/blob/master/Child.c))

```c
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
```
Instead of invoking the *add* function, the **Child** version of the **Object** structure will invoke the *sub* function. Notice how the *buildChild* function invokes *build* of **Object** and then overrides the *myFunction* function pointer.

Now we have an **Object** that acts like a **Child**, yet almost no one needs to know. The only entity that needs to know is the one calling *buildChild* function instead of the *build* function. This switch can be executed dynamically at runtime. Looking at main.c we can see that the choice between the *build* or the *buildChild* functions is performed dynamically based on a random variable.
```c
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
``` 

[Dependency injection](http://martinfowler.com/articles/injection.html) is exactly what is going on inside the *print* function. At runtime, when the *buildChild* function is selected, the **Child** variant of **Object** is being injected into the *print* function. This is interesting because the type of the parameter is mandatory ([aka closed](http://en.wikipedia.org/wiki/Open/closed_principle)) but its behavior is not ([aka open](http://en.wikipedia.org/wiki/Open/closed_principle)).

The **Child** variant of **Object** can be developed years after **Object** was written, by completely different developers, yet it will still work inside *print* function without requiring modifications to the *print* function. 

![](http://www.plantuml.com/plantuml/svg/bL9PRiCW5Fpd54_-IgIQ0qIH5Qa7w1dWWGID0OkbLHNbxiMGs4rY9oeVBFDcXc4vXCXyJAOd91or7PXd1aBywMNklSPQKbf8CD9mwGCXZdzAZD0OfYt51bY0SJhX7sMagbISkiUhmk64lolTYmiTrHrSsOZJjax1YK1kEL_Aw-hwY01FMR_HDW9xWt7WMtAURV1YKmX2R6x5RdkYb9uBuyjf0OFFn2Uabtj3vSu9SF4WLLhElfeT_sOFYVylreBtd6ryI6Y5-xPjDrxr3fhzNbliav2XwvfR0YuGyiMsQLXHPy5QvIfRyYG7QKN-YRy0)

Not only does this allows the **Object**'s *print* function to invoke the **Child**'s *sub* function without depending on it, but to do so **Child** is the one depending on the **Object**.




