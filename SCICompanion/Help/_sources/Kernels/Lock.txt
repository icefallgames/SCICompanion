.. Lock

.. default - domain::js

.. include:: /includes/standard.rst

=============
Lock (Kernel)
=============


.. function:: Lock(resourceType resourceNumber [lock])

	.. IMPORTANT:: SCI1.1 only.

	Locks or unlocks resources of a certain type. Locking presuambly prevents a resource
	from being unloaded, so it is likely a performance optimization.

	:param number resourceType: A resource type, e.g. rsVIEW, rsPIC, etc...
	:param number resourceNum: A resource number, or $ffff for all resources of that type.
	:param boolean lock: If TRUE or unspecified, the resource(s) is locked. Otherwise they are locked.


