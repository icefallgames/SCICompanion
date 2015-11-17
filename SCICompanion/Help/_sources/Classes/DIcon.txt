.. DIcon

.. default - domain::js

.. include:: /includes/standard.rst

===========================
DIcon (of :class:`Control`)
===========================

.. class:: DIcon

	Defined in DialogControls.sc.

	An icon control.


Subclasses: :class:`DCIcon`.

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
		DIcon [color=greenyellow]
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

Defined in DIcon:

======== =============
Property Description  
======== =============
view     The icon view
loop     The icon loop
cel      The icon cel 
======== =============


Methods
==========

.. function:: setSize()
	:noindex:



