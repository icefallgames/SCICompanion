.. ControlIcon

.. default - domain::js

.. include:: /includes/standard.rst

==================================
ControlIcon (of :class:`IconItem`)
==================================

.. class:: ControlIcon

	Defined in GameControls.sc.

	
	Extends :class:`IconItem` by having a object and selector.
	
	When clicked (selected), the *selector* property will be invoked on *theObj*.
	
	Example::
	
		(instance iconRestart of ControlIcon
			(properties
				view 995
				loop 4
				cel 0
				nsLeft 80
				nsTop 82
				message 8
				signal $01c3
				noun N_RESTART
				helpVerb V_HELP
			)
		)
	
		// then later...
	
		(iconRestart:
			theObj(gGame)
			selector(#restart)
		)
	


.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		IconItem -> ControlIcon
		IconItem -> Slider
		IconItem -> InventoryItem
		ControlIcon [color=greenyellow]
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
============== ===========

Defined in ControlIcon:

======== =======================================================
Property Description                                            
======== =======================================================
theObj   An object that gets notified when this icon is pressed.
selector A method selector (e.g. #doit) on theObj.              
======== =======================================================


Methods
==========

.. function:: doit()
	:noindex:



.. function:: select(fSelect)
	:noindex:



