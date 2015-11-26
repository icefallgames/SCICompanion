.. Rest

.. include:: /includes/standard.rst

================
 Rest
================

**rest** allows you to create and use function with variably sized parameters.
It is very useful in forwarding the parameters of one function to another.

Example::

	(procedure (SomeProcedure aFixedParam params) 
		(var i) 
		(SomeOtherProcedure(rest params)) // this would be call SomeOtherProcedure starting with params 
	)

	// Then in another function, you could call it like this... 
	(SomeProcedure(someParam))
	(SomeProcedure(someParam anotherParam oneMoreParam))

A good example of where this is used is :class:`Actor`'s setMotion method. In this case, you pass the class name
of a :class:`Motion` class to setMotion, followed by parameters to initialize that Motion object. The key point is
that the setMotion implementation doesn't need to be aware of the number and type of the parameters that the Motion class's
init method requires::

	(method (setMotion theMover sendParams))
		...
		= mover (send theMover:new())
		// Call the mover's init method with myself (the Actor), followed
		// by whatever additional parameters were passed.
		(send mover:init(self rest sendParams))
	)

So now to set a Mover on an Actor, you can do::

	(mySpecialActor:setMotion(MoveTo 34 126))

And setMotion will end up calling the MoveTo's init method, forwarding 34 and 126::

	(method (init theClient x y)
		// theClient is the Actor
		// x and y will be 34 and 136.
	)

