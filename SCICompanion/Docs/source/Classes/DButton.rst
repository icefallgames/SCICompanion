.. DButton

.. default - domain::js

.. include:: /includes/standard.rst

=============================
DButton (of :class:`Control`)
=============================

.. class:: DButton

	Defined in DialogControls.sc.

	A button control.


Subclasses: :class:`DColorButton`.

.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		Control -> DText
		Control -> DButton
		Control -> DSelector
		Control -> DEdit
		Control -> DIcon
		DIcon -> DCIcon
		DSelector -> FileSelector
		DButton -> DColorButton
		DButton [color=greenyellow]
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
name                                                   
======== ==============================================

Defined in DButton:

======== ================
Property Description     
======== ================
text     The button text.
font     The button font.
======== ================


Methods
==========

.. function:: dispose(param1)
	:noindex:



.. function:: setSize()
	:noindex:



