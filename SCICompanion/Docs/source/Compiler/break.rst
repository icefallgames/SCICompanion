.. Break and Breakif statement

.. include:: /includes/standard.rst

================================
 break and breakif statements
================================

Use break to exit from a **for**, **while** or **repeat** loop, or breakif to exit from the loop conditionally.

syntax::

	(break)

	(breakif conditional_expression)

Example using the break statement::

	(= GAME_RUNNING TRUE) 
	(repeat
		(self doit:) 
		(if (not GAME_RUNNING)
			(break) ; if GAME_RUNNING == FALSE, the loop will stop
		)
	)

Example using the breakif statement::

	(= GAME_RUNNING TRUE) 
	(repeat
		(self doit:) 
		(breakif (not GAME_RUNNING)) ; if GAME_RUNNING == FALSE, the loop will stop
	)


See also: :doc:`for`, :doc:`while`, :doc:`repeat`, :doc:`continue`.