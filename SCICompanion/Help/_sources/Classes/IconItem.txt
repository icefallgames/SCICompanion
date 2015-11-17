.. IconItem

.. default - domain::js

.. include:: /includes/standard.rst

==========================
IconItem (of :class:`Obj`)
==========================

.. class:: IconItem

	Defined in IconItem.sc.

	
	An icon that represents an action. These are used, for instance, in the main icon bar, or in the inventory dialog.
	
	If you wish to do something in response to the icon being clicked, override the select(params) method in your IconItem instance like so::
	
	    (method (select params)
	        (if ((super:select(rest params)))
	            // Do something here....
	        )
	        return 0
	    )


Subclasses: :class:`ControlIcon`, :class:`Slider`, :class:`InventoryItem`.

.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		IconItem -> ControlIcon
		IconItem -> Slider
		IconItem -> InventoryItem
		IconItem [color=greenyellow]
	}

Properties
==========

Defined in IconItem:

============== =======================================
Property       Description                            
============== =======================================
view           The view for the icon                  
loop           The loop for the icon                  
cel            The cel for the icon                   
nsLeft                                                
nsTop                                                 
nsRight                                               
nsBottom                                              
state                                                 
cursor         Cursor number associated with the icon.
type                                                  
message        A verb (e.g. V_LOOK) if type is evVERB.
modifiers                                             
signal                                                
maskView                                              
maskLoop                                              
maskCel                                               
highlightColor                                        
lowlightColor                                         
noun           The noun associated with this icon.    
modNum                                                
helpVerb                                              
============== =======================================


Methods
==========


.. function:: show([left top])
	:noindex:

	:param number left: Optional parameter to set left position of icon.
	:param number top: Optional parameter to set top position of icon.




.. function:: select([fProcessEvents])
	:noindex:

	:param boolean fProcessEvents: If TRUE, sets its state based on consuming mouse release events. If unspecified, just selects the control.
	:returns: TRUE if the icon was selected, FALSE otherwise.



.. function:: highlight(param1)
	:noindex:



.. function:: onMe(param1)
	:noindex:



.. function:: mask()
	:noindex:



