.. RestoreGame

.. default - domain::js

.. include:: /includes/standard.rst

====================
RestoreGame (Kernel)
====================

.. function:: RestoreGame(gameName slotNumber gameVerison)

	
	Restores a game state with the game saved in the specified slot number. The gameName and gameVersion parameters are used to prevent opening of games from other versions.
	
	:param string gameName: The name of the game.
	:param number slotNumber: The slot number of the save game.
	:param string gameVersion: The game version.
	
	Example::
	
		(RestoreGame "LSL3" 2 "1.002")


