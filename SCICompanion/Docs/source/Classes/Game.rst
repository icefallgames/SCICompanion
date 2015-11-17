.. Game

.. default - domain::js

.. include:: /includes/standard.rst

======================
Game (of :class:`Obj`)
======================

.. class:: Game

	Defined in Game.sc.

	
	This contains the base game functionality. Most of the time, these methods are just called by the framework.
	The main interesting ones you might need to call from code are:
	
	handsOn()
		Gives the user control of the game.
	
	handsOff()
		Removes control of the game from the user (e.g. for cutscenes)
	
	setCursor()
		Sets the current game cursor.


Subclasses: :class:`SQ5`.

Properties
==========

Defined in Game:

============= =====================================
Property      Description                          
============= =====================================
script                                             
printLang                                          
_detailLevel                                       
panelObj      Current object target of a UI control
panelSelector Selector for the object target.      
handsOffCode                                       
handsOnCode                                        
============= =====================================


Methods
==========

.. function:: init()
	:noindex:



.. function:: doit()
	:noindex:



.. function:: play()
	:noindex:



.. function:: replay()
	:noindex:



.. function:: newRoom(newRoomNumber)
	:noindex:

	
	Changes to a new room.
	
	.. IMPORTANT::
	    To change to a new room, you should *not* use this method. Instead, use the newRoom method on the current room, e.g::
	
	    	(send gRoom:newRoom(105))
	


.. function:: startRoom(param1)
	:noindex:



.. function:: restart()
	:noindex:



.. function:: restore(param1)
	:noindex:



.. function:: save()
	:noindex:



.. function:: changeScore(param1)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: showMem()
	:noindex:



.. function:: setCursor(cursorNumber isVisible cursorX cursorY hotspotX hotspotY)
	:noindex:



.. function:: notify()
	:noindex:



.. function:: setScript(theScript sendParams)
	:noindex:



.. function:: cue()
	:noindex:



.. function:: quitGame(param1)
	:noindex:



.. function:: masterVolume(param1)
	:noindex:



.. function:: detailLevel(the_detailLevel)
	:noindex:



.. function:: pragmaFail()
	:noindex:



.. function:: handsOff(param1)
	:noindex:

	
	Removes control of the game from the user (for cutscenes).
	
	Example usage::
	
		(send gGame:handsOff())
	


.. function:: handsOn(param1)
	:noindex:

	
	Gives the user control of the game.
	
	Example usage::
	
		(send gGame:handsOn())
	


