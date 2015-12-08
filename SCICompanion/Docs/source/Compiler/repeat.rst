.. Repeat statement

.. include:: /includes/standard.rst

==================
 repeat statement
==================

Use **repeat** to execute a sequence of instructions over and over. Use :doc:`break` to break out of the loop.

syntax::

	(repeat
		code_block_1
		code_block_2
		...
	)

The code blocks are continually executed until a break or return statement is encountered, or a breakif statement is satisfied::

	(repeat
		(= nextControl (dialog next:))
		(breakif (not nextControl))
	)


See also: :doc:`while`, :doc:`for`, :doc:`break`.