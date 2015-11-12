.. Variables

.. include:: /includes/standard.rst

================
 Variables
================


SCI Scripts have four different types of variables: **global**, **local**, **temporary** and **parameter**.

**Local variables** are defined in the local script block. There can only be one local script block in each script.
These variables can only be accessed from the script they are defined in, unless the script is script.000 (Main.sc).
The Main.sc local variables are **global variables**, and can be used in any other scripts as long as (use "Main") is specified.

An example of declaring and using local or global variables::

	(local
		// no value defined, set to zero 
		ZeroVariable 
		// A normal variable 
		SomeLocal = 30 
		// 30 word sized variables (60 bytes total), all set to zero 
		LocalArray[30] 
		// The six variables in the array are set, the rest will be zero 
		DeclaredArray[10] = (0 1 2 $ABCD 3 4) 
	)

	(procedure (VarProc) 
		// Read a variable normally 
		(if(== SomeLocal 20) 
		// do something 
		)

		// Use a variable for looping and set 
		// the first 20 entries in LocalArray to 0-19 
		(for (= SomeLocal 0) (< SomeLocal 20) (++SomeLocal) 
		(= LocalArray[SomeLocal] SomeLocal) 
		)

		// An example of using a variable for strings 
		(= SomeLocal "Hello") 
		// gets the address of SomeLocal 
		Display(@SomeLocal) 
	)


**Temporary variables** can only be used in the method or procedure they are defined in.
To declare temporary variables, you must use the var keyword *at the beginning* of the function.

An example of declaring temporary variables::

	(procedure (VarProc)
		// Multiple variables can be defined, and they cn have preset values
		(var TempVar1 = 30, TempVar2, TempVar3, TempVar4 = 99)
	)

**Parameter variables** can only be used in the method or procedure they are defined in. These variables are defined in the function definition.

An example of declaring parameters::

	// Procedure with three parameter variables:
	(procedure (PrintingProc String X Y) 
		Display(String dsCOORD X Y) 
	)

	// Calling this procedure:
	(procedure (SomeProc)
		PrintingProc("Hello World" 60 80)
	)



 