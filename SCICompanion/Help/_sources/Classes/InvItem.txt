.. InvItem

.. default - domain::js

.. include:: /includes/standard.rst

==========================
InvItem (of :class:`InvI`)
==========================

.. class:: InvItem

	Defined in InvItem.sc.

	
	This extends class:`InvI` and adds the ability to automatically print messages (for the *look* and *do* verbs) that include a visual representation of the inventory item.
	
	An example inventory item might look like::
	
		(instance Hammer of InvItem
			(properties
				view 900
				loop 1
				cursor 900			// Optional cursor when using this item.
				message V_HAMMER	// Optional verb associated with the item.
				signal $0002
				noun N_HAMMER		// noun from message resource 0
			)
		)
	


Properties
==========

Inherited from :class:`InvI`:

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
owner                     
script                    
value                     
============== ===========


Methods
==========

.. function:: doVerb(theVerb)
	:noindex:



