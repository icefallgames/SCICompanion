.. AddToScore

.. default - domain::js

.. include:: /includes/standard.rst

====================
AddToScore (Main.sc)
====================

.. function:: AddToScore(flag amount)

	
	Adds an amount to the player's current score. A flag (one used with
	:func:`BSet`, :func:`BClear` or :func:`BTest`) must be provided. This
	ensures that a score is only added once.
	
	:param number flag: A flag indicating what this score is for.
	:param number amount: The amount to add to the score.


