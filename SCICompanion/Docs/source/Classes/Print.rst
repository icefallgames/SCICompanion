.. Print

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Print (of :class:`Object`)
==========================

.. class:: Print

	Defined in Print.sc.

	
	The Print class lets you show text, buttons and images in a GUI on screen.
	
	It is generally used as a static class, in the sense that you call methods on Print
	directly without making an instance of it. The init: method then displays the dialog.
	
	Example usage::
	
		(Print
			addTitle: "This is the title"
			font: 0
			width: 90
			addText: "Enter new font number:"
			addEdit: @temp0 6 0 24
			addButton: 0 "A button!" 0 12
			init:
		)
	


Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Print:

========== ===============================================
Property   Description                                    
========== ===============================================
dialog                                                    
window                                                    
title      The dialog title.                              
mode       Default alignment: alLEFT, alCENTER or alRIGHT.
font                                                      
width                                                     
x                                                         
y                                                         
ticks                                                     
caller                                                    
retValue                                                  
modeless                                                  
first                                                     
saveCursor                                                
========== ===============================================


Methods
==========


.. function:: init([theCaller text])
	:noindex:

	Calls showSelf() to show the dialog.

	:param heapPtr theCaller: An optional object with a cue method.
	:param string text: Optional text that is added to the dialog.
	:returns: If a button was pressed, returns the value of that button.



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: showSelf()
	:noindex:

	Shows the dialog. Generally, you should use the init() method to show the dialog.



.. function:: addButton(theValue noun verb cond seq [x y modNum])
	:noindex:

.. function:: addButton(theValue text [x y])
	:noindex:

	Adds a button to the dialog, either with the supplied text or a message resource. 

	:param number theValue: A numerical value associated with the button. This value will be returned by the dialog if the button is pressed.
	:param string text: The button text.
	:param number x: The button x offset from the upper left of the dialog.
	:param number y: The button y offset from the upper left of the dialog.
	:param number noun: The noun of the message to show.
	:param number verb: The verb of the message to show.
	:param number cond: The condition of the message to show.
	:param number seq: The sequence of the message to show, or 0.
	:param number modNum: Optional room number of the message to show (if not specified, the current room is used).




.. function:: addColorButton(theValue noun verb cond seq [x y modNum normalForeColor highlightForeColor selectedForeColor normalBackColor highlightBackColor selectedBackColor])
	:noindex:
.. function:: addColorButton(theValue text [x y normalForeColor highlightForeColor selectedForeColor normalBackColor highlightBackColor selectedBackColor])
	:noindex:

	This is similar to addButton, but lets you specify up to 6 color indices for various parts and states of the button.

	:param number theValue: A numerical value associated with the button. This value will be returned by the dialog if the button is pressed.
	:param string text: The button text.
	:param number x: The button x offset from the upper left of the dialog.
	:param number y: The button y offset from the upper left of the dialog.
	:param number noun: The noun of the message to show.
	:param number verb: The verb of the message to show.
	:param number cond: The condition of the message to show.
	:param number seq: The sequence of the message to show, or 0.
	:param number modNum: Optional room number of the message to show (if not specified, the current room is used).




.. function:: addEdit(buffer maxLength [x y initialText])
	:noindex:

	Adds an edit control to the dialog.

	:param string buffer: The buffer that will receive the text.
	:param number maxLength: The length of the buffer.
	:param number x: The edit control x offset from the upper left of the dialog.
	:param number y: The edit control y offset from the upper left of the dialog.
	:param string initialText: Text to initialize the edit control.




.. function:: addIcon(view loop cel [x y])
	:noindex:

	Adds an icon to the dialog.




.. function:: addText(noun verb cond seq [x y modNum])
	:noindex:

.. function:: addText(text [x y])
	:noindex:

	Adds text to the dialog, either with the supplied string or a message resource. 

	:param string text: The text.
	:param number x: The text x offset from the upper left of the dialog.
	:param number y: The text y offset from the upper left of the dialog.
	:param number noun: The noun of the message to show.
	:param number verb: The verb of the message to show.
	:param number cond: The condition of the message to show.
	:param number seq: The sequence of the message to show, or 0.
	:param number modNum: Optional room number of the message to show (if not specified, the current room is used).



.. function:: addTextF(params)
	:noindex:

	Adds formatted text to the dialog. There are no options for positioning or getting the text from a message resource.



.. function:: addTitle(noun verb cond seq [modNum])
	:noindex:

.. function:: addTitle(text)
	:noindex:

	Adds a title to the dialog, either with the supplied string or a message resource. 

	:param string text: The title text.
	:param number noun: The noun of the message to show.
	:param number verb: The verb of the message to show.
	:param number cond: The condition of the message to show.
	:param number seq: The sequence of the message to show, or 0.
	:param number modNum: Optional room number of the message to show (if not specified, the current room is used).



.. function:: posn(theX theY)
	:noindex:

	Positions the dialog on screen.


.. function:: handleEvent(pEvent)
	:noindex:



.. function:: cue()
	:noindex:



