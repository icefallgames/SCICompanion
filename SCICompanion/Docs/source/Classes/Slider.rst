.. Slider

.. default - domain::js

.. include:: /includes/standard.rst

=============================
Slider (of :class:`IconItem`)
=============================

.. class:: Slider

	Defined in Slider.sc.

	
	This is used for slider controls in the GUI, such as a volume slider or game speed slider.


.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		IconItem -> ControlIcon
		IconItem -> Slider
		IconItem -> InventoryItem
		Slider [color=greenyellow]
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

Defined in Slider:

=========== ===========
Property    Description
=========== ===========
sliderView             
sliderLoop             
sliderCel              
sTop                   
sLeft                  
sRight                 
maxY                   
minY                   
underBits              
yStep                  
theObj                 
selector               
bottomValue            
topValue               
=========== ===========


Methods
==========

.. function:: doit(sendParams)
	:noindex:




.. function:: show([left top])
	:noindex:

	:param number left: Optional parameter to set left position of slider.
	:param number top: Optional parameter to set top position of slider.



.. function:: select(fSelect)
	:noindex:



.. function:: highlight()
	:noindex:



.. function:: advance()
	:noindex:



.. function:: retreat()
	:noindex:



.. function:: move(delta passValue)
	:noindex:



.. function:: valueToPosn(theValue)
	:noindex:



.. function:: posnToValue(position)
	:noindex:



