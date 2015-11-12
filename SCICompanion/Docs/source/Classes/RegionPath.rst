.. RegionPath

.. default - domain::js

.. include:: /includes/standard.rst

===============================
RegionPath (of :class:`MoveTo`)
===============================

.. class:: RegionPath

	Defined in RegionPath.sc.

	
	RegionPath is used to describe a path that an :class:`Actor` follows through a region. A
	region can span many rooms. They can be used to give the illusion that a single character
	is wandering around an entire region.
	
	The region points are provided in a buffer that looks like the following::
	
		(local
			localPathPoints[15] = ($7fff room1 x1 y1 x2 y2 x3 y3 $7fff room2 x1 y1 x2 y2 $8000)
		)
	
	$7fff signals a new room, and $8000 signals the end of the buffer.		
	You are responsible for serving up value from this buffer by overriding the **at** method.
	
	The exact usage of this class is still unknown. Hopefully someone will write a tutorial that describes it well.


Properties
==========

Inherited from :class:`MoveTo`:

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
========= ===========

Defined in RegionPath:

============= ===========
Property      Description
============= ===========
currentRoom              
value                    
endType                  
intermediate             
initialized              
savedOldStuff            
theRegion                
theOldBits               
theOldSignal             
============= ===========


Methods
==========

.. function:: init(theClient theCaller theIntermediate)
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: next()
	:noindex:



.. function:: atEnd()
	:noindex:



.. function:: nextRoom()
	:noindex:



.. function:: at()
	:noindex:



.. function:: nextValue()
	:noindex:



.. function:: findPathend()
	:noindex:



.. function:: findPrevroom()
	:noindex:



.. function:: curRoomCheck()
	:noindex:



