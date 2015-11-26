.. Rest

.. include:: /includes/standard.rst

================
 &rest
================

**&rest** allows you to create and use function with variably sized parameters.
It is very useful in forwarding the parameters of one function to another.

Example::

	(procedure (SomeProcedure aFixedParam) 
		(SomeOtherProcedure &rest) ; this would be call SomeOtherProcedure with the params passed after aFixedParam
	)

	; Then in another function, you could call it like this... 
	(SomeProcedure someParam)
	(SomeProcedure someParam anotherParam oneMoreParam) ; anotherParam and oneMoreParam would be forwarded on

A good example of where this is used is :class:`Actor`'s setMotion method. In this case, you pass the class name
of a :class:`Motion` class to setMotion, followed by parameters to initialize that Motion object. The key point is
that the setMotion implementation doesn't need to be aware of the number and type of the parameters that the Motion class's
init method requires::

	(method (setMotion theMover))
		...
		= mover (send theMover:new())
		; Call the mover's init method with myself (the Actor), followed
		; by whatever additional parameters were passed.
		(send mover:init(self &rest))
	)

So now to set a Mover on an Actor, you can do::

	(mySpecialActor setMotion: MoveTo 34 126)

And setMotion will end up calling the MoveTo's init method, forwarding 34 and 126::

	(method (init theClient x y)
		; theClient is the Actor
		; x and y will be 34 and 136.
	)

If you need to reference one of the forwarded parameters explicitly, then you can supply a optional parameter name to &rest::

	(method (myMethod theX theY theZ theMass)
		(= x theX)
		(= y theY)
		(= z theZ)
		(= mass theMass)
		; pass theZ, theMass and any other supplies parameters onto CalculateGravity:
		(CalculateGravity &rest theZ)
	)