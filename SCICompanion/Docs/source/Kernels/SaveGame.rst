.. SaveGame

.. default - domain::js

.. include:: /includes/standard.rst

=================
SaveGame (Kernel)
=================

.. function:: SaveGame(gameName slotNumber description gameVersion)

	
	Saves a game state to disk. It saves it in the slot number specified with the given description. The gameName and gameVersion parameters are used to prevent opening of games from other versions.
	
	:param string gameName: The name of the game.
	:param number slotNumber: The slot number of the save game.
	:param string gameVersion: The game version.
	:returns: TRUE on success, FALSE on failure.
	
	Example::
	
		(SaveGame "LSL3" 2 "Some game saved in slot 2" "1.002")


