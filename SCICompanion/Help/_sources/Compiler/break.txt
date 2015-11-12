.. Break statement

.. include:: /includes/standard.rst

=================
 Break statement
=================

Use break to exit from a loop.

syntax::

	break

Example using the break statement::

	(= GAME_RUNNING TRUE) 
	(while (TRUE) // while(TRUE) causes an infinite loop
		(self:doit()) 
		(if( not GAME_RUNNING)
			break // if GAME_RUNNING == FALSE, the loop will stop
		) 
	)

See also: :doc:`for`, :doc:`while`, :doc:`if`, :doc:`do`.