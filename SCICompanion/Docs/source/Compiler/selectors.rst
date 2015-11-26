.. Selectors

.. include:: /includes/standard.rst

================
 Selectors
================

Selectors are identifiers for *properties* and *methods* of classes and instances.
Each property and method has a numerical selector index, which points to its name in vocab.997. Normally you don't need to worry about the numerical selector indices.

Examples of selectors are: x, y, doit, init, dispose, changeState.

To retrieve a property for an object, use the object's name, followed by a space, then selector name with a question mark (?) appended.

Setting a property takes the same form as a method call: Use the object's name, followed by a space, then the selector name with a colon (:) appended.
Following that come the arguments passed to the selector (the value to set the property to, or the parameters for the method).

You can invoke multiple selectors on an object by separating the selector calls with commas (,).

Examples::

	; Using the x selector to retrieve a property:
	(Printf "The ego's x is %d" (gEgo x?))

	; Using the z selector to set a property on the myView object:
	(myView z: 10)

	; Calling a method selector on the myView object:
	(myView init:)

	; Calling multiple selectors on the myActor object:
	(myActor
		setCycle: Forward,
		setMotion: MoveTo 100 30,
		init:
	)

You are also able to retrieve the numerical selector index of a property or method selector. To do this, you can use the # character::

	(Printf "The init selector index is %d" #init)

One place this is useful is when you want to test if an object supports a particular property or method::

	; Test if the object has a cue method before calling that cue method:
	(if (theObject respondsTo: #cue)
		(theObject cue:)
	)


