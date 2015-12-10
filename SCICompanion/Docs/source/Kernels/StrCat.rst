.. StrCat

.. default - domain::js

.. include:: /includes/standard.rst

===============
StrCat (Kernel)
===============

.. function:: StrCat(dest src)

	
	Appends src to dest and returns a pointer to dest.
	
	Example::
	
		(procedure (SomeFunc &tmp [aString 40] strPtr)
			(StrCpy aString "Hello World")
			(StrCat aString ", How Are You?") ; aString will be "Hello World, How Are You?"
		)

