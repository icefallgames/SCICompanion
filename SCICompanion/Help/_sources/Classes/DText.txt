.. DText

.. default - domain::js

.. include:: /includes/standard.rst

===========================
DText (of :class:`Control`)
===========================

.. class:: DText

	Defined in Controls.sc.

	A control that simply displays text.


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
		DText [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Control`:

======== ==============================================
Property Description                                   
======== ==============================================
type                                                   
state                                                  
nsTop                                                  
nsLeft                                                 
nsBottom                                               
nsRight                                                
key      The keyboard key associated with this control.
said                                                   
value    Arbitrary value associated with this control. 
======== ==============================================

Defined in DText:

======== ============================
Property Description                 
======== ============================
text     The text.                   
font     The font.                   
mode     alLEFT, alCENTER or alRIGHT.
rects                                
======== ============================


Methods
==========

.. function:: new()
	:noindex:



.. function:: dispose(param1)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: setSize(param1)
	:noindex:



.. function:: draw()
	:noindex:



