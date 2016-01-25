.. InventoryItem

.. default - domain::js

.. include:: /includes/standard.rst

====================================
InventoryItem (of :class:`IconItem`)
====================================

.. class:: InventoryItem

	Defined in InventoryItem.sc.

	
	This extends :class:`IconItem` and adds the ability to automatically print messages (for the *look* and *do* verbs) that include a visual representation of the inventory item.
	It also provides an owner property that indicates who owns the item.
	
	An example inventory item might look like::
	
		(instance Hammer of InventoryItem
			(properties
				view 900
				loop 1
				cursor 900			; Optional cursor when using this item.
				message V_HAMMER	; Optional verb associated with the item.
				signal $0002
				noun N_HAMMER		; noun from message resource 0
			)
		)


.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		IconItem -> ControlIcon
		IconItem -> Slider
		IconItem -> InventoryItem
		InventoryItem [color=greenyellow]
	}

Properties
==========

Inherited from :class:`IconItem`:

============== ===========
Property       Description
============== ===========
view                      
loop                      
cel                       
nsLeft                    
nsTop                     
nsRight                   
nsBottom                  
state                     
cursor                    
type                      
message                   
modifiers                 
signal                    
maskView                  
maskLoop                  
maskCel                   
highlightColor            
lowlightColor             
noun                      
modNum                    
helpVerb                  
name                      
============== ===========

Defined in InventoryItem:

======== ===========
Property Description
======== ===========
owner               
script              
value               
======== ===========


Methods
==========

.. function:: show()
	:noindex:



.. function:: highlight(param1)
	:noindex:



.. function:: onMe(param1)
	:noindex:



.. function:: ownedBy(theObj)
	:noindex:



.. function:: moveTo(theOwner)
	:noindex:



.. function:: doVerb(theVerb)
	:noindex:



