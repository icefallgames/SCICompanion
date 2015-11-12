.. Selectors

.. include:: /includes/standard.rst

================
 Selectors
================

Selectors are identifiers for properties and methods of classes and instances.
Each property and method has a selector index, which points to its name in vocab.997.

Examples of selectors are: x, y, doit, init, dispose, changeState.

Example::

	// Using the x selector to retrieve a property:
	FormatPrint("The x is %d" (send gEgo:x))

	// Using the z selector to set a property:
	(myView:z(10))

	// Calling a method selector:
	(myView:init())

However, you are also able to retrieve the numerical selector index of a property or method selector. To do this, you can use the # character::

	FormatPrint("The init selector index is %d" #init)

One place this is useful is when you want to test if an object supports a particular property or method::

	// Test if the object has a cue method before calling that cue method:
	(if (send theObject:respondsTo(#cue))
		(send theObject:cue())
	)


