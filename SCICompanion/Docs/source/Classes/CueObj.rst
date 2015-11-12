.. CueObj

.. default - domain::js

.. include:: /includes/standard.rst

===========================
CueObj (of :class:`Script`)
===========================

.. class:: CueObj

	Defined in Feature.sc.

	
	This is a static Script object that works in conjunction with :class:`Feature` when making the
	ego move close to the Feature in response to interacting with the Feature. When the ego is in range, the
	action is then repeated.
	
	Generally, you shouldn't need to bother with this class.


Properties
==========

Inherited from :class:`Script`:

=========== ===========
Property    Description
=========== ===========
client                 
state                  
start                  
timer                  
cycles                 
seconds                
lastSeconds            
ticks                  
lastTicks              
register               
script                 
caller                 
next                   
=========== ===========

Defined in CueObj:

======== ===========
Property Description
======== ===========
theVerb             
======== ===========


Methods
==========

.. function:: changeState(newState)
	:noindex:



