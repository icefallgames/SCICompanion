.. Variables

.. include:: /includes/standard.rst

================
 Variables
================


SCI Scripts have four different types of variables: **global**, **local**, **temporary** and **parameter**.

**Local variables** are defined in the local script block. There can only be one local script block in each script.
These variables can only be accessed from the script they are defined in, unless the script is script.000 (Main.sc).
The Main.sc local variables are **global variables**, and can be used in any other scripts as long as **(use Main)** is specified.

An example of declaring and using local or global variables::

	(local
		; no value defined, set to zero 
		ZeroVariable 
		; A normal variable 
		SomeLocal = 30 
		; An array of 30 word sized variables (60 bytes total), all set to zero 
		[LocalArray 30] 
		; The first six variables in the array are set, the remaining four will be zero 
		[DeclaredArray 10] = [0 1 2 $ABCD 3 4]
		; This array is sized depending on how many values it is initialized with (in this case four)
		ImplicitArray = [1 10 20 30]
	)

	(procedure (VarProc) 
		; Read a variable normally 
		(if (== SomeLocal 20) 
			; do something 
		)

		; Use a variable for looping and set 
		; the first 20 entries in LocalArray to 7
		(for ((= SomeLocal 0)) (< SomeLocal 20) ((++ SomeLocal)) 
			(= [LocalArray SomeLocal] 7) 
		)

		; An example of using a variable for strings 
		(= SomeLocal "Hello") 
		; gets the address of SomeLocal 
		(Display @SomeLocal) 
	)


**Temporary variables** can only be used in the method or procedure they are defined in.
To declare temporary variables, you add a **&tmp** token to the parameter list, and declare the temporary variables
following that. Just like local variables, temporary variables can be arrays. However, no initial values may be supplied.

An example of declaring temporary variables::

	(procedure (VarProc &tmp TempVar1 TempVar2 TempVar3 [SomeBuffer 10])
		; You can't specify initializers in the temp variable list, so assign explicitly:
		(= TempVar 1)
	)

**Parameter variables** can only be used in the method or procedure they are defined in. These variables are defined in the function definition.

An example of declaring parameters::

	// Procedure with three parameter variables:
	(procedure (PrintingProc theString X Y) 
		(Display theString dsCOORD X Y) 
	)

	// Calling the above procedure:
	(procedure (SomeProc)
		(PrintingProc "Hello World" 60 80)
	)



 