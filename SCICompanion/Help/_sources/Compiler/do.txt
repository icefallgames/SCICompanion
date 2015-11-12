.. Do loop

.. include:: /includes/standard.rst

================
 Do loop
================

Use do to implement a conditional loop.

syntax::

	(do
		code_block_1
	) while (conditional_expression)

code_block_1 is executed once. If conditional_expression evaluates to be TRUE (non zero), code block 1 is executed again, and continually until conditional_expression evaluates to be FALSE (zero).

Example using the do statement::

	(= GAME_RUNNING TRUE) 
	(do
	  (self:doit()) 
	) while(GAME_RUNNING)

See also: :doc:`for`, :doc:`while`, :doc:`if`, :doc:`break`.