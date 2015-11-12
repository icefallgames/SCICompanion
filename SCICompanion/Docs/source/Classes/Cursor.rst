.. Cursor

.. default - domain::js

.. include:: /includes/standard.rst

========================
Cursor (of :class:`Obj`)
========================

.. class:: Cursor

	Defined in Obj.sc.

	The Cursor class represents an onscreen mouse cursor.


Properties
==========

Defined in Cursor:

======== ===========
Property Description
======== ===========
view                
loop                
cel                 
x                   
y                   
hotSpotX            
hotSpotY            
hidden              
======== ===========


Methods
==========

.. function:: init()
	:noindex:

	Initializes the cursor and sets it as the active cursor.


.. function:: posn(x y)
	:noindex:

	Sets the position of the cursor.


.. function:: posnHotSpot(theHotSpotX theHotSpotY)
	:noindex:

	Sets the hotspot of the cursor.


.. function:: setLoop(theLoop)
	:noindex:

	Changes the view loop of the cursor.


.. function:: setCel(theCel)
	:noindex:

	Changes the cel of the cursor.


.. function:: showCursor(show)
	:noindex:

	Shows or hides the cursor.


