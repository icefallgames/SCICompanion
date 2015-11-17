.. DColorButton

.. default - domain::js

.. include:: /includes/standard.rst

==================================
DColorButton (of :class:`DButton`)
==================================

.. class:: DColorButton

	Defined in DColorButton.sc.

	
	A Button control that lets you set various colors for different states.


.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		Control -> DText
		Control -> DSelector
		Control -> DButton
		Control -> DEdit
		Control -> DIcon
		DIcon -> DCIcon
		DButton -> DColorButton
		DSelector -> FileSelector
		DColorButton [color=greenyellow]
	}

Properties
==========

Inherited from :class:`DButton`:

======== ===========
Property Description
======== ===========
type                
state               
nsTop               
nsLeft              
nsBottom            
nsRight             
key                 
said     Unused     
value               
text                
font                
======== ===========

Defined in DColorButton:

======== ============================
Property Description                 
======== ============================
mode     alLEFT, alCENTER or alRIGHT.
nfc      Normal foreground color.    
nbc      Normal background color.    
hfc      Highlight foreground color. 
hbc      Highlight background color. 
sfc      Selected foreground color.  
sbc      Selected background color.  
======== ============================


Methods
==========

.. function:: track(param1)
	:noindex:



.. function:: setSize(param1)
	:noindex:



.. function:: draw()
	:noindex:



