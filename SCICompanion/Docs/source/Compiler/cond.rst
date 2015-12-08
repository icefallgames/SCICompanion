.. Cond statements

.. include:: /includes/standard.rst

================
 cond statements
================

Use **cond** to conditionally execute different pieces of code depending on a condition that applies to each.
Only the first condition that is satisfied will have its containing code executed.

syntax::

	(cond
		(conditional_expression_1
			code_block_1
			code_block_2
			...
		)
		(conditional_expression_2
			code_block_1
			code_block_2
			...
		)
		...
		[(else
			code_block_1
			code_block_2
			...
		)]
	)


Example of using the cond statement::

	(cond
		((< (gEgo x?) 100)
			(Prints "You're on the left side of the screen")
		)
		((> (gEgo x?) 220)
			(Prints "You're on the right side of the screen")
		)
		(gIsWearingBombSuit
			(Prints "You're close to the explosion, but your bomb suit keeps you safe")
		)
		(else
			(Prints "You're too close to the explosion, you die!")
			(Die BLEW_UP)
		)
	)

See also: :doc:`switch`, :doc:`switchto`, :doc:`if`.