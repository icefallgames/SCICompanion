.. Switchto statement

.. include:: /includes/standard.rst

====================
 switchto statement
====================

The **switchto** statement is similar to the :doc:`switch`, except that the case values are implied. This
is useful if you're frequently inserting and removing cases from a changeState method, since you won't
need to update the case values.

syntax::

	(switchto switch_expression
		(
			code_block_1
			code_block_2
			...
		)
		(
			code_block_1
			code_block_2
			...
		)
		. 
		. 
		. 
		(
			code_block_1
			code_block_2
			...
		)
	)

Example using the switchto statement::

	(switchto state
		( 
			(= seconds 2) ; Wait two seconds
		) 
		( 
			(client setMotion: MoveTo 100 120 self) ; Move to (100, 120) then go to next state
		) 
		( 
			(drumRoll play:)	; play the drum rool
			(= seconds 3)
		) 
		(
			(drumRool stop:)	; and stop after three seconds.
		) 
	) 

See also: :doc:`if`, :doc:`switch`, :doc:`cond`.