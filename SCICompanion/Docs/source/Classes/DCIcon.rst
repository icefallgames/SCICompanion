.. DCIcon

.. default - domain::js

.. include:: /includes/standard.rst

==========================
DCIcon (of :class:`DIcon`)
==========================

.. class:: DCIcon

	Defined in DCIcon.sc.

	
	This is a dialog icon that animates ("dialog cycle icon").


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
		DCIcon [color=greenyellow]
	}

Properties
==========

Inherited from :class:`DIcon`:

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
said                
value               
view                
loop                
cel                 
name                
======== ===========

Defined in DCIcon:

========== ===========
Property   Description
========== ===========
cycler                
cycleSpeed            
signal                
========== ===========


Methods
==========

.. function:: init()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: cycle()
	:noindex:



.. function:: lastCel()
	:noindex:



