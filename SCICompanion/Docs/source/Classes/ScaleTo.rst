.. ScaleTo

.. default - domain::js

.. include:: /includes/standard.rst

============================
ScaleTo (of :class:`Scaler`)
============================

.. class:: ScaleTo

	Defined in ScaleTo.sc.

	
	ScaleTo allows you to grow or shrink an object over time.
	
	Example usage::
	
		// Scale the portal down to 50% of its normal size.
		(portal:setScale(ScaleTo 50))


Properties
==========

Inherited from :class:`Scaler`:

========= ===========
Property  Description
========= ===========
client               
frontY               
backY                
frontSize            
backSize             
slopeNum             
slopeDen             
const                
========= ===========

Defined in ScaleTo:

============= ===========
Property      Description
============= ===========
caller                   
endScale                 
step                     
waitCount                
scaleDir                 
saveWaitCount            
============= ===========


Methods
==========


.. function:: init(theClient theEndScale [theCaller])
	:noindex:

.. function:: init(theClient theEndScale theStep [theCaller])
	:noindex:

.. function:: init(theClient theEndScale theStep theWaitCount [theCaller])
	:noindex:

	Initializes the ScaleTo.

	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param number theEndScale: The final scale of the Prop in percentage of full size.
	:param number theStep: The amount the scale increases or decreases each cycle. Smaller values are smoother and slower.
	:param number theWaitCount: The number of ticks to wait before the scaling begins.
	:param heapPtr theCaller: The object that will be cue()'d when the final scale size is reached.



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:



