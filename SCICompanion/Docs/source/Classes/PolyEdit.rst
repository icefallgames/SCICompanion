.. PolyEdit

.. default - domain::js

.. include:: /includes/standard.rst

===========================
PolyEdit (of :class:`List`)
===========================

.. class:: PolyEdit

	Defined in PolygonEdit.sc.

	
	This is an in-game polygon editor. It is not essential for the template
	game, but it is included for historical purposes.
	
	Usage::
	
		(PolyEdit:doit())


Properties
==========

Inherited from :class:`List`:

======== ===========
Property Description
======== ===========
elements            
size                
name                
======== ===========

Defined in PolyEdit:

=========== ===========
Property    Description
=========== ===========
curPolygon             
x                      
y                      
state                  
isMouseDown            
curMenu                
undoPrvPoly            
undoPoly               
undoPolyBuf            
undoX                  
undoY                  
undoState              
=========== ===========


Methods
==========

.. function:: init()
	:noindex:



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: add()
	:noindex:



.. function:: delete(param1)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: changeState(newState)
	:noindex:



.. function:: draw()
	:noindex:



.. function:: select(param1 param2)
	:noindex:



.. function:: selectPt()
	:noindex:



.. function:: addPt()
	:noindex:



.. function:: finishAdding()
	:noindex:



.. function:: movePt(param1 param2)
	:noindex:



.. function:: insertPt()
	:noindex:



.. function:: deletePt()
	:noindex:



.. function:: undo()
	:noindex:



.. function:: saveForUndo(param1)
	:noindex:



.. function:: advanceRetreat(param1 param2)
	:noindex:



.. function:: readObstacles()
	:noindex:



.. function:: writeObstacles()
	:noindex:



.. function:: showMap(param1)
	:noindex:



.. function:: exit()
	:noindex:



