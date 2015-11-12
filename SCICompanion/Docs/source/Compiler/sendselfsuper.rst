.. Send, Self and Super

.. include:: /includes/standard.rst

======================
 Send, Self and Super
======================

Send, self and super are used to call methods in :doc:`classes`.

send
	can call methods from any class or instance.

self
	calls methods from it's own class or instance's methods.

super
	calls methods from it's superclass.

Syntax::

	(send <ClassName>:
		<MethodName>([params])
		...
	)

If you refer directly to an instance of class, you do not need the **send** keyword.

Examples::

	// you can access classes directly like this... 
	(ClassX: 
		init() 
		doit() 
	)

	// you can call it directly... 
	(SomeInstance:foo())

	// or indirectly...
	= InstancePtr SomeInstance 
	(send InstancePtr:foo())

	// or a shortcut to the indirect way...
	(send (= InstancePtr SomeInstance):foo())

	(self:init())

	(super:doit())
