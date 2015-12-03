.. For loop

.. include:: /includes/standard.rst

================
 For loop
================

Use for to implement a conditional loop.

syntax::

	(for (code_block_1) conditional_expression (code_block_3) 
		code_block_2
	)

code_block_1 is executed once. while conditional expression evaluates to be TRUE (non zero), code_block_2 and then code_block_3 is executed.

Example using the for statement - prints "." on the screen 20 times::

	(for ((= i 0)) (< i 20) ((++ i))
		(Display "." dsCOLOR clLIME)
	)

It is important to note that the conditional_expression is *not* enclosed in parentheses (unless it is an expression other than a plain value),
while code_block_1 and code_block_3 *are* enclosed in parentheses.
This is because the code blocks may contain multiple statements. For example::

	; This loop starts with i = 5 and j = 6, and will execute 6 times.
	(for ((= i 5) (= j 6)) (or i j) ((-- i) (-- j))
		(Printf "i: %d, j: %d" i j)
	)
	; Prints out:
	; i: 5, j: 6
	; i: 4, j: 5
	; i: 3, j: 4
	; i: 2, j: 3
	; i: 1, j: 2
	; i: 0, j: 1

See also: :doc:`if`, :doc:`while`, :doc:`do`, :doc:`break`.