.. DisposeLoad

.. default - domain::js

.. include:: /includes/standard.rst

============================
DisposeLoad (DisposeLoad.sc)
============================

.. function:: DisposeLoad(resourceType resourceNumber)

	
	This procedure can be used to preload a series of resources in a room so there is no delay when 
	they are actually used. With modern hardware, there is probably no reason to use this in your game.
	
	It can also be used to dispose of loaded scripts if resourceType is set to zero. This functionality is
	used by the disposeCode.sc script.


