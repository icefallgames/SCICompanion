.. Wait

.. default - domain::js

.. include:: /includes/standard.rst

=============
Wait (Kernel)
=============

.. function:: Wait(ticks)

	
	This waits (halts the game) for the specified number of ticks. Since the interpreter is on 60Hz beats, a Wait(60) is equivalent to one second.
	It returns the time passed between the end of the last Wait() call, and the current one.
	
	:param number ticks: The number of ticks to wait.
	
	Example::
	
		Wait(120) // Waits for two seconds
	
	See also: :func:`GetTime`.


