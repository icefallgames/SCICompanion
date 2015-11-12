.. For loop

.. include:: /includes/standard.rst

================
 For loop
================

Use for to implement a conditional loop.

syntax::

	(for (code_block_1) (conditional_expression) (code_block_3) 
		code_block_2
	)

code_block_1 is executed once. while conditional expression evaluates to be TRUE (non zero), code_block_2 and then code_block_3 is executed.

Example using the for statement - prints "." on the screen 20 times::

	(for (= i 0) (< i 20) (++i)
		Display("." dsCOLOR clLIME)
	)

See also: :doc:`if`, :doc:`while`, :doc:`do`, :doc:`break`.