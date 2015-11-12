.. IconI

.. default - domain::js

.. include:: /includes/standard.rst

=======================
IconI (of :class:`Obj`)
=======================

.. class:: IconI

	Defined in IconI.sc.

	
	An icon that represents an action. These are used, for instance, in the main icon bar, or in the inventory dialog.


Subclasses: :class:`ControlIcon`, :class:`InvI`, :class:`Slider`.

Properties
==========

Defined in IconI:

============== =======================================
Property       Description                            
============== =======================================
view           The view for the icon                  
loop           The loop for the icon                  
cel            The cel for the icon                   
nsLeft                                                
nsTop                                                 
nsRight                                               
nsBottom                                              
state                                                 
cursor         Cursor number associated with the icon.
type                                                  
message        A verb (e.g. V_LOOK) if type is evVERB.
modifiers                                             
signal                                                
maskView                                              
maskLoop                                              
maskCel                                               
highlightColor                                        
lowlightColor                                         
noun           The noun associated with this icon.    
modNum                                                
helpVerb                                              
============== =======================================


Methods
==========


.. function:: show([left top])
	:noindex:

	:param number left: Optional parameter to set left position of icon.
	:param number top: Optional parameter to set top position of icon.



.. function:: select(param1)
	:noindex:



.. function:: highlight(param1)
	:noindex:



.. function:: onMe(param1)
	:noindex:



.. function:: mask()
	:noindex:



