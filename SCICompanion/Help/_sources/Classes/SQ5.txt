.. SQ5

.. default - domain::js

.. include:: /includes/standard.rst

======================
SQ5 (of :class:`Game`)
======================

.. class:: SQ5

	Defined in Main.sc.

	
	The main game class. This subclasses :class:`Game` and adds game-specific functionality.


Properties
==========

Inherited from :class:`Game`:

============= ===========
Property      Description
============= ===========
script                   
printLang                
_detailLevel             
panelObj                 
panelSelector            
handsOffCode             
handsOnCode              
============= ===========


Methods
==========

.. function:: init()
	:noindex:

	
	Modify this to set up any initial state the game needs. Among the things set here are:
	
	- The maximum score.
	- Text colors and fonts used in messages.
	- The action icons.
	- The default game cursor.
	
	


.. function:: doit(param1)
	:noindex:



.. function:: play()
	:noindex:



.. function:: startRoom(param1)
	:noindex:



.. function:: restart(param1)
	:noindex:



.. function:: restore(param1)
	:noindex:



.. function:: save(param1)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:

	
	Modify this method to change any global keyboard bindings.
	


.. function:: setCursor(cursorNumber param2 param3 param4)
	:noindex:



.. function:: quitGame(param1)
	:noindex:



.. function:: pragmaFail()
	:noindex:

	
	Modify this method to add any default messages for actions.
	


.. function:: handsOff()
	:noindex:

	
	This disables player control (e.g. for cutscenes).
	


.. function:: handsOn(fRestore)
	:noindex:

	
	This re-enables player control after having been disabled.
	


.. function:: showAbout()
	:noindex:



.. function:: showControls()
	:noindex:



