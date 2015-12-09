.. FileSelector

.. default - domain::js

.. include:: /includes/standard.rst

====================================
FileSelector (of :class:`DSelector`)
====================================

.. class:: FileSelector

	Defined in FileSelector.sc.

	A dialog selector control that displays a list of files.


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
		FileSelector [color=greenyellow]
	}

Properties
==========

Inherited from :class:`DSelector`:

========= ===========
Property  Description
========= ===========
type                 
state                
nsTop                
nsLeft               
nsBottom             
nsRight              
key                  
said                 
value                
font                 
x                    
y                    
text                 
cursor               
topString            
mark                 
name                 
========= ===========

Defined in FileSelector:

======== ===========
Property Description
======== ===========
mask                
nFiles              
sort                
======== ===========


Methods
==========

.. function:: dispose()
	:noindex:



.. function:: setSize()
	:noindex:



.. function:: readFiles(theMask)
	:noindex:

	Reads files from the current directory with the specified filter, or \*.\* if none is specified.


