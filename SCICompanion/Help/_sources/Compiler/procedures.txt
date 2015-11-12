.. Procedures

.. include:: /includes/standard.rst

================
 Procedures
================

Procedures are an alternative to methods. Both have the same functionality, but procedures are not attatched to classes or instances.
They can, however, be designed for use with a specific class by specifying the class which will be calling it.

Specifying a public procedure will make the procedure available to be used by any other script in you game. Just use the "use" keyword to attatch the scripts.
In SCI1.1, you also need to give it an export index, as described in :doc:`version`.


Syntax::

	(procedure [public](<ProcedureName> [<ParamName1> <ParamName2> ... (optional)]) [of <ClassName>]
		<code>
	)

Examples::

	(procedure (SomeProc)
		PrintingProc("Hello World" 60 80)
	)

	/* This is a public procedure so any other script can use it*/
	(procedure public (PrintingProc String X Y) 
		Display(String dsCOORD X Y) 
	)

	/* An example of using it with a class */

	(class SomeClass
		(properties
			top 20
			left 0
			bottom 100
			right 40
		)
		(method (someMethod)
			someProc()
		)
	)

	(procedure (someProc) of SomeClass
		= top 50 /* sets SomeClass:top to 50 */
		= right 100 /* sets SomeClass:right to 100 */
	)