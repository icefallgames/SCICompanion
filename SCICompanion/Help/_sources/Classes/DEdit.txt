.. DEdit

.. default - domain::js

.. include:: /includes/standard.rst

===========================
DEdit (of :class:`Control`)
===========================

.. class:: DEdit

	Defined in DialogControls.sc.

	An edit control.


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
		DEdit [color=greenyellow]
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

Defined in DEdit:

======== =========================================
Property Description                              
======== =========================================
text                                              
font     The edit control font.                   
max      The maximum number of characters allowed.
cursor                                            
======== =========================================


Methods
==========

.. function:: track(param1)
	:noindex:



.. function:: setSize()
	:noindex:



