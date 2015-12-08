.. Self and Super

.. include:: /includes/standard.rst

======================
 Self and Super
======================

**self** and **super** are special keywords than can be used to call methods on the current object or the parent object without directly specifying the class name.
You can also use them to pass a reference to the current object to another method or procedure.

self
	calls methods from its own class or instance's methods.

super
	calls methods from its superclass.

Syntax::

	(self <Selector>: [params])
	(super <Selector>: [params])

Examples::

	; call the init method on myself:
	(self init:)

	; call the doit method on my super class:
	(super doit:)

	; pass a reference to myself to another method:
	(gEgo setMotion: MoveTo 100 150 self)
