.. ControlItem

.. default - domain::js

.. include:: /includes/standard.rst

=====================================
ControlItem (of :class:`ControlIcon`)
=====================================

.. class:: ControlItem

	Defined in SettingsPane.sc.

	
	Represents an icon with functionality that can be used in a GUI.
	
	When pressed, the *selector* property will be invoked on *theObj*.


Properties
==========

Inherited from :class:`ControlIcon`:

============== =======================================================
Property       Description                                            
============== =======================================================
view                                                                  
loop                                                                  
cel                                                                   
nsLeft                                                                
nsTop                                                                 
nsRight                                                               
nsBottom                                                              
state                                                                 
cursor                                                                
type                                                                  
message                                                               
modifiers                                                             
signal                                                                
maskView                                                              
maskLoop                                                              
maskCel                                                               
highlightColor                                                        
lowlightColor                                                         
noun                                                                  
modNum                                                                
helpVerb                                                              
theObj         An object that gets notified when this icon is pressed.
selector       A method selector (e.g. #doit) on theObj.              
============== =======================================================


Methods
==========

.. function:: select(param1)
	:noindex:



