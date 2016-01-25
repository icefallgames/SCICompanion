.. DebugRoomInit

.. default - domain::js

.. include:: /includes/standard.rst

================================
DebugRoomInit (DebugRoomInit.sc)
================================

.. function:: DebugRoomInit(roomNumber)

	
	Modify this function to set up inventory items or flags as necessary.
	
	:param number roomNumber: The room that the game is starting in when being debugged.
	
	Example::
	
		(150 ; The room number
			; The ego must have the thing if he's in this room.
			(gEgo set: INV_THETHING)
		)
	
	See also: :doc:`/debugging`


