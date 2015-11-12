.. AimToward

.. default - domain::js

.. include:: /includes/standard.rst

===================
AimToward (Main.sc)
===================


.. function:: AimToward(theObj otherObj [cueObj])

.. function:: AimToward(theObj otherObj faceEachOther [cueObj])
	:noindex:

.. function:: AimToward(theObj x y [cueObj])
	:noindex:

	Changes the heading of theObj so it faces another object or position.
	Optionally causes the other object to face the first object.

	:param heapPtr theObj: The object that is being aimed.
	:param heapPtr otherObj: The target object.
	:param boolean faceEachOther: If TRUE, the otherObj will also be made to face theObj.
	:param number x: The target x.
	:param number y: The target x.
	:param heapPtr cueObj: Optional object to be cue()'d.


