.. Inset

.. default - domain::js

.. include:: /includes/standard.rst

========================
Inset (of :class:`Code`)
========================

.. class:: Inset

	Defined in Inset.sc.

	
	An Inset lets you create a small window on the screen which can contain its own
	cast, and respond to its own events and such.


Properties
==========

Inherited from :class:`Code`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Inset:

============== ===========
Property       Description
============== ===========
picture                   
anOverlay                 
style                     
view                      
loop                      
cel                       
x                         
y                         
priority                  
register                  
hideTheCast               
caller                    
owner                     
script                    
oldCast                   
oldFeatures               
oldATPs                   
oldMH                     
oldKH                     
oldDH                     
oldWH                     
oldObstacles              
oldStyle                  
inset                     
disposeNotOnMe            
modNum                    
noun                      
insetView                 
============== ===========


Methods
==========

.. function:: init(theCaller theOwner theRegister)
	:noindex:

	
	Initializes the Inset. Generally you would override this to initialize your Inset
	with the necessary Props and such. Then call (super:init(rest params)).
	
	:param heapPtr theCaller: Object that gets cue()'d when the Inset is disposed.
	:param heapPtr theOwner: Generally a room.
	


.. function:: doit()
	:noindex:



.. function:: dispose(param1)
	:noindex:



.. function:: setScript(theScript sendParams)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: doVerb(theVerb)
	:noindex:



.. function:: hideCast(param1)
	:noindex:



.. function:: drawInset()
	:noindex:



.. function:: restore()
	:noindex:



.. function:: refresh()
	:noindex:




.. function:: setInset([theInset theCaller theRegister])
	:noindex:

	Sets an :class:`Inset` on this Inset! To clear the inset, pass no parameters.

	:param heapPtr theInset: The Inset instance.
	:param heapPtr theCaller: An object that will get cue()'d when the Inset is disposed.

	Example usage::

		(send myInset:setInset(anotherSubInset))




.. function:: onMe(theObj)
	:noindex:

.. function:: onMe(x y)
	:noindex:

If the Inset has a view, return true if the object is on that view.

	:param heapPtr theObj: An object with x and y properties.
	:param number x: The x coordinate.
	:param number y: The y coordinate.
	:returns: TRUE if the object is on the Inset's view. If there is no view, returns TRUE.



