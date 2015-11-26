.. If statements

.. include:: /includes/standard.rst

================
 If statements
================

Use if to implement a conditional statement.

syntax::

	(if (conditional_expression)
		code_block_1
	)
	Optionally...
	(else
		code_block_2
	)

If conditional_expression evaluates to be true (non zero), code_block_1 is executed, otherwise it is skipped. If an else is appended to the if statement and conditional expression evaluates to be false (zero), code_block_2 is executed.


Examples of using the if statement::

	(if(HaveMouse()) 
		DrawStatus("Mouse is available!") 
	)(else 
		DrawStatus("Mouse is NOT available!") 
	)

	(if (SomeVar or (SomeOtherVar and not AnotherVar)) 
		// do something 
	)

 
 Ternary operator
 ===================

 If statements can also be evaluated (like the ternary operator (?:) in c++).

 Example::
 
	// Set myVar to 5 if we've opened the sewer. Otherwise set it to 10.
	(= myVar
		(if (Btest(FLAG_OpenedSewer))
			5
		)(else
			10
		)
	)



See also: :doc:`for`, :doc:`while`, :doc:`do`, :doc:`break`.