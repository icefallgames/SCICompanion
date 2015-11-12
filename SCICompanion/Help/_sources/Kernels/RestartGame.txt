.. RestartGame

.. default - domain::js

.. include:: /includes/standard.rst

====================
RestartGame (Kernel)
====================

.. function:: RestartGame()

	
	Restarts the game. In the sense that the interpreter was an actual computer, this would be like pressing the reset button.
	It restarts the game by stopping all the sounds, disposing of the menubar, clearing the heap status, and disposing of all the scripts.
	Then, it starts executing the game again. Because of all of this, it doesn't return a value.
	
	See also: :func:`GameIsRestarting`, :func:`SaveGame`, :func:`RestoreGame`.


