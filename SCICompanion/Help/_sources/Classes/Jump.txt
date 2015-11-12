.. Jump

.. default - domain::js

.. include:: /includes/standard.rst

=========================
Jump (of :class:`Motion`)
=========================

.. class:: Jump

	Defined in Jump.sc.

	
	This serves as a base class for the :class:`JumpTo` motion.


Subclasses: :class:`JumpTo`.

Properties
==========

Inherited from :class:`Motion`:

========= =============================================================
Property  Description                                                  
========= =============================================================
client    The :class:`Actor` to which this is attached.                
caller    The object that will get cue()'d when the motion is complete.
x                                                                      
y                                                                      
dx                                                                     
dy                                                                     
b-moveCnt                                                              
b-i1                                                                   
b-i2                                                                   
b-di                                                                   
b-xAxis                                                                
b-incr                                                                 
completed                                                              
xLast                                                                  
yLast                                                                  
========= =============================================================

Defined in Jump:

=========== ===========
Property    Description
=========== ===========
gx                     
gy                     
xStep                  
yStep                  
signal                 
illegalBits            
waitApogeeX            
waitApogeeY            
=========== ===========


Methods
==========

.. function:: init(theClient theCaller)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: motionCue()
	:noindex:



.. function:: setTest()
	:noindex:



