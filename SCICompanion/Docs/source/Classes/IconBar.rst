.. IconBar

.. default - domain::js

.. include:: /includes/standard.rst

=========================
IconBar (of :class:`Set`)
=========================

.. class:: IconBar

	Defined in IconItem.sc.

	
	A class that manages the icon bar at the top of the screen in the template game.


Subclasses: :class:`InventoryBase`, :class:`GameControls`.

Properties
==========

Inherited from :class:`Set`:

======== ===========
Property Description
======== ===========
elements            
size                
name                
======== ===========

Defined in IconBar:

=============== ===========
Property        Description
=============== ===========
height                     
underBits                  
oldMouseX                  
oldMouseY                  
curIcon                    
highlightedIcon            
prevIcon                   
curInvIcon                 
useIconItem                
helpIconItem               
walkIconItem               
port                       
window                     
state                      
activateHeight             
y                          
=============== ===========


Methods
==========

.. function:: doit()
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: show()
	:noindex:



.. function:: hide()
	:noindex:



.. function:: advance()
	:noindex:



.. function:: retreat()
	:noindex:




.. function:: select(theCurIcon [fProcessEvents])
	:noindex:



.. function:: highlight(theHighlightedIcon param2)
	:noindex:



.. function:: swapCurIcon()
	:noindex:



.. function:: advanceCurIcon()
	:noindex:



.. function:: dispatchEvent(pEvent)
	:noindex:



.. function:: disable(param1)
	:noindex:



.. function:: enable(param1)
	:noindex:



.. function:: noClickHelp()
	:noindex:



.. function:: findIcon(param1)
	:noindex:



