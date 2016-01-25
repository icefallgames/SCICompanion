.. Room

.. default - domain::js

.. include:: /includes/standard.rst

=========================
Room (of :class:`Region`)
=========================

.. class:: Room

	Defined in Game.sc.

	
	Room is the class that your room instances should inherit from.


Properties
==========

Inherited from :class:`Region`:

=========== ===============================================================
Property    Description                                                    
=========== ===============================================================
name                                                                       
script      Room script, generally set with setScript in the init() method.
number                                                                     
modNum                                                                     
noun        Noun associated with the room background.                      
_case       Case associated with the room background.                      
timer                                                                      
keep                                                                       
initialized                                                                
=========== ===============================================================

Defined in Room:

========== =============================================================
Property   Description                                                  
========== =============================================================
picture    The number of the pic resource for the room background.      
style      The dp\* animation flags that control room opening animation.
horizon    y coordinate of room's horizon.                              
controls                                                                
north      The room to the north (if appropriate)                       
east       The room to the east (if appropriate)                        
south      The room to the south (if appropriate)                       
west       The room to the west (if appropriate)                        
curPic                                                                  
picAngle   How far from vertical is the view? 0-89                      
vanishingX                                                              
vanishingY                                                              
obstacles                                                               
inset                                                                   
========== =============================================================


Methods
==========

.. function:: init()
	:noindex:

	
	Override this method to provide the needed initialization for your room. Make sure to call (super:init()).
	


.. function:: doit()
	:noindex:

	
	Override this method to provide custom logic that is executed every game cycle for your room. Make sure you call (super:doit()).
	


.. function:: dispose()
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:

	
	Override this method to provide custom event handling logic for your room. Make sure you call (super:handleEvent(pEvent)).
	


.. function:: newRoom(newRoomNumber)
	:noindex:

	
	This tells the room that we want to switch to a new room. This is the main way to switch rooms.
	
	Example usage::
	
		(gRoom newRoom: 145)
	


.. function:: setRegions(scriptNumbers)
	:noindex:

	
	Lets you indicate which regions this room is part of.
	
	:param number ScriptNumbers: One or more script numbers for scripts that contain a public :class:`Rgn` instance.
	
	Example usage::
	
		(method (init)
			(self setRegions: SNOW_REGION MOUNTAIN_REGION)
			; etc, do more room initialization...
			(super init:)
		)
	



.. function:: drawPic(picNumber [picAnimation])
	:noindex:

	Draws a pic background.

	:param number picNumber: The number of the pic resource.
	:param number picAnimation: An optional dp\* enum value (e.g. dpOPEN_INSTANTLY).

	See the :func:`DrawPic` kernel for more information on animation flags.




.. function:: overlay(picNumber [picAnimation])
	:noindex:

	Overlays an additional pic on top of the current background.

	:param number picNumber: The number of the pic resource.
	:param number picAnimation: An optional dp\* enum value (e.g. dpOPEN_INSTANTLY).



.. function:: addObstacle(polygon sendParams)
	:noindex:

	
	This can be used to add polygon obstacles to the current room.
	
	Example usage::
	
		(gRoom addObstacle: (CreateNewPolygon @P_Flower))
	


.. function:: reflectPosn(theActor theEdgeHit)
	:noindex:

	
	Assigns the actor a new position based on the edge that it left from
	in the previous room.
	


.. function:: edgeToRoom(theEdgeHit)
	:noindex:

	
	:param number theEdgeHit: EDGE_TOP, EDGE_LEFT, EDGE_RIGHT or EDGE_BOTTOM.
	:returns: The room number associated with that edge.
	


.. function:: roomToEdge(roomNumber)
	:noindex:

	
	:param number roomNumber: A room number.
	:returns: The edge associated with that room number (or the room number if no edge associated).
	



.. function:: setInset([theInset theCaller theRegister])
	:noindex:

	Sets an :class:`Inset` on this room. To clear the inset, pass no parameters.

	:param heapPtr theInset: The Inset instance.
	:param heapPtr theCaller: An object that will get cue()'d when the Inset is disposed.

	Example usage::

		(gRoom setInset: cageCloseUp self)



