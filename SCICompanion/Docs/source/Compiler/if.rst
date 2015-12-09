.. if statements

.. include:: /includes/standard.rst

================
 if statements
================

Use if to implement a conditional statement.

syntax::

	(if conditional_expression
		code_block_1
		code_block_2
		...
	[else]
		[code_block_1]
		[code_block_2]
		[...]
	)

If conditional_expression evaluates to be true (non zero), the subsequent code (up to the *else*) is executed.
If an else clause is present, the code after the *else* will be excuted if conditional_expression evaluates to zero.


Examples of using the if statement::

	(if (HaveMouse)		; HaveMouse is a procedure call
		(DrawStatus "Mouse is available!") 
	else 
		(DrawStatus "Mouse is NOT available!") 
	)

	(if (or SomeVar (and SomeOtherVar (not AnotherVar)) )
		; do something
	)

Note that the if condition is *not* enclosed in parentheses unless it is itself an expression. If it's just a plain value, then it looks like::

	(if someVariable
		(Prints "someVariable is TRUE")
	)
 
 Ternary operator
 ===================

 If statements can also be evaluated (like the ternary operator (?:) in c++).

 Example::
 
	; Set myVar to 5 if we've opened the sewer. Otherwise set it to 10.
	(= myVar
		(if (Btest FLAG_OpenedSewer) 5 else 10)
	)


See also: :doc:`switch`, :doc:`switchto`, :doc:`cond`.