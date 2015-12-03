.. While statement

.. include:: /includes/standard.rst

================
 While statement
================

Use while to implement a conditional loop.

syntax::

	(while conditional_expression
		code_block_1
		code_block_2
		...
	)

The code blocks are continually executed until conditional_expression evaluates to be FALSE (zero)::

	(= counter 10)
	; This loop will execute ten times, unless counter is 0:
	(while counter
		(Printf "Counting down: %d" counter)
		(-- counter)
	)

See also: :doc:`repeat`, :doc:`for`, :doc:`break`.