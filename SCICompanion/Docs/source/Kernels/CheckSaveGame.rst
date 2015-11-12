.. CheckSaveGame

.. default - domain::js

.. include:: /includes/standard.rst

======================
CheckSaveGame (Kernel)
======================

.. function:: CheckSaveGame(gameName slotNumber)

.. function:: CheckSaveGame(gameName slotNumber gameVersion)
	:noindex:

	
	Checks to see if a particular saved game can be loaded.
	
	:param string gameName: The user-specified name of the game.
	:param number slotNumber: The game slot.
	:param string gameVerision: Optional parameter.
	:returns: TRUE on success, FALSE on fail.
	
	.. NOTE::
		This makes sure the saved game exists, is valid, and matches the gameName and gameVersion.


