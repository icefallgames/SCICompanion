.. Procedures

.. include:: /includes/standard.rst

================
 Procedures
================

Procedures are an alternative to methods. Both have the same functionality, but procedures are not attached to classes or instances.
They can, however, be declared inside a class. In that case, the procedure can only be called from code within the class
and can reference class properties and class-specific tokens like *self* and *super*.

You can make a procedure public (usable by other scripts) by listing it in the :doc:`public <public>` section of a script and giving it an export index.

You can declare temporary variables (local to the procedure) by including a &tmp token after the procedure parameters, and following it with
a list of temp variable declarations.

Syntax::

	(procedure (<ProcedureName> [<ParamName1> <ParamName2> ... &tmp <TempVar1> <TempVar2> ...])
		<code>
	)

Examples::

	(procedure (SomeProc)
		(PrintingProc "Hello World" 60 80)
	)

	; This is a public procedure so any other script can use it
	(public
		PrintingProc 0
	)
	(procedure (PrintingProc String X Y &tmp result) 
		(= result (+ 1 2 3))
		(Display String dsCOORD X Y) 
	)

	; An example of using it with a class
	(class SomeClass
		(properties
			top 20
			left 0
			bottom 100
			right 40
		)
		(method (someMethod)
			(someProc)
		)

		(procedure (someProc)
			(= top 50)		; sets SomeClass:top to 50
			(= right 100)	; sets SomeClass:right to 100
		)
	)
