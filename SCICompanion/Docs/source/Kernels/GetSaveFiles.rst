.. GetSaveFiles

.. default - domain::js

.. include:: /includes/standard.rst

=====================
GetSaveFiles (Kernel)
=====================

.. function:: GetSaveFiles(gameName descBuffer descSlots)

	
	Retrieves the total number of save files and their descriptions.
	The descriptions of the saved games are stored in the descBuffer. The slot numbers of the saved game strings' saved games are stored in the descSlots array.
	
	:param string gameName: The name of the game.
	:param string descBuffer: Buffer where descriptions of the games will be written.
	:returns: The number of saved games, or -1 if there was an error.


