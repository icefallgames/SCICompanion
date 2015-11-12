.. RelDPath

.. default - domain::js

.. include:: /includes/standard.rst

============================
RelDPath (of :class:`DPath`)
============================

.. class:: RelDPath

	Defined in RelDPath.sc.

	
	This is similar to :class:`DPath`, but the points are relative to each other.
	
	Example usage::
	
		// Move the turtle 10 down, 10 to the right, and then 10 up.
		(turtle:setMotion(RelDPath 0 10 10 0 0 -10 self))


Properties
==========

Inherited from :class:`DPath`:

========= ===========
Property  Description
========= ===========
client               
caller               
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
points               
value                
========= ===========


Methods
==========

.. function:: setTarget()
	:noindex:



