The Locust Programming Language
===============================
This project defines the Locust programming language, and provides you with the
necessary tools to compile source code written in Locust form to machine code,
as well as other things.

Goals of Locust
---------------
Locust aims to be an extremely simple and readable programming language. In
order to achieve this, we've taken some of the more esoteric symbols commonly
experienced in the c-family of languages and converted them to a more literal
syntax.

### Types ###
Another goal of Locust is to provide the flexibility to work for those who like
dynamically typed languages, and those who prefer statically typed. In order to
achieve this, we're implementing a gradual type system, wherein unspecified
variables are dynamically typed by default, and uses type inferrence to obtain
the type of the value the variable refers to. For instance:

	x = 34

Static typing is simply obtained by specifying the type desired like so.

	int32(y) = 5
	
With a statically typed variable, there will be no implicit coercion. So be sure
to assign the right kind of value for your type, or we'll error on you *so* hard.

If you want to coerce a value, you simply apply the same principle to the value
side of the assignment.

	z = 0.5
	z = int64(z)
	
Here, z is an object, that first held a 32 bit floating point value 0.5, but
on the second line, we coerced z's value to a 64 bit integer and truncated
0.5 to 0.


### Paradigms ###
Locust makes heavy use of coroutines, which are a more expressive superset of normal functions.
There's nothing special needed to make use of them as a language user, and even actors are coroutines.

One of the benefits of using coroutines is that it makes scheduling much easier, and the language maintains
a strict 1:1 ratio between threads and active virtual processors reported by the OS. In this way, the actors
and coroutines are multiplexed as effectively as the Locust scheduler is able to, without OS context switching.

### Concurrency ###
Locust will use the actor model to achieve concurrency. Each module is an actor,
and nothing is shared between actors, except what is explicitly passed in at
creation. You can create your own actors like so:

    actor baz(foo, oof, x, y, z)
		tell parent foo(x, y, z)
		tell parent oof(x, y, z)

Here we created the actor baz, and sent it a message containing foo, oof, x, y,
and z from our current context. baz, on initialization sent two messages back,
and then reached the end of its tasks. When this happens, the actor will remove
itself from scheduling. If it were to start a loop, however, then each iteration
through the loop is scheduled separately, and it will not remove itself from
scheduling.

License
-------
Locust is released under the MIT License, you can find the actual text in
LICENSE in the root repository directory.
