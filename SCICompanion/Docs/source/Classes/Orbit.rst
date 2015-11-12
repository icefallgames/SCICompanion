.. Orbit

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Orbit (of :class:`Motion`)
==========================

.. class:: Orbit

	Defined in Orbit.sc.

	
	A motion class that moves an Actor in an orbit around another object.


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

Defined in Orbit:

========= ===========
Property  Description
========= ===========
centerObj            
radius               
xTilt                
yTilt                
angleStep            
winding              
curAngle             
========= ===========


Methods
==========

.. function:: init(theClient theCenterObj theRadius theXTilt theYTilt theAngleStep theWinding theCurAngle)
	:noindex:



.. function:: moveDone()
	:noindex:



