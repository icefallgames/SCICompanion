.. DSelector

.. default - domain::js

.. include:: /includes/standard.rst

===============================
DSelector (of :class:`Control`)
===============================

.. class:: DSelector

	Defined in DialogControls.sc.

	A selector control that lets the player choose from a list of text strings.


Subclasses: :class:`FileSelector`.

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

Defined in DSelector:

========= ===========
Property  Description
========= ===========
font                 
x                    
y                    
text                 
cursor               
topString            
mark                 
========= ===========


Methods
==========

.. function:: handleEvent(pEvent)
	:noindex:



.. function:: setSize()
	:noindex:



.. function:: indexOf(findText)
	:noindex:

	
	Returns the index of specified string in the list of items. If the string is not an item in the list, it returns -1.	
	
	:param string findText: The text to search for.
	


.. function:: at(index)
	:noindex:

	
	Returns the text at the specified position in the list.
	
	:param number index: The index into the list.
	:returns: A pointer to the text.
	


.. function:: advance(amount)
	:noindex:

	Increments the selected item's index by amount.


.. function:: retreat(amount)
	:noindex:

	Decrements the selected item's index by amount.


