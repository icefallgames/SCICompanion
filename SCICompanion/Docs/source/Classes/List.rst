.. List

.. default - domain::js

.. include:: /includes/standard.rst

=============================
List (of :class:`Collection`)
=============================

.. class:: List

	Defined in System.sc.

	The List class is an extension of the Collection class. It is used to handle a list of elements such as objects that can be accessed by index.


Subclasses: :class:`Controls`, :class:`Dialog`, :class:`Set`, :class:`Conversation`, :class:`_EditablePolygon`, :class:`PolyEdit`, :class:`DialogEditor`.

Properties
==========

Inherited from :class:`Collection`:

======== ===========
Property Description
======== ===========
elements            
size                
name                
======== ===========


Methods
==========

.. function:: showStr(buffer)
	:noindex:



.. function:: at(theIndex)
	:noindex:

	Returns a pointer to the node at the specified index.


.. function:: last()
	:noindex:

	Returns a pointer to the last node in the collection.


.. function:: prev(node)
	:noindex:

	Returns a pointer to the node before the specified one in the collection.



.. function:: addToFront([nodes ...])
	:noindex:

	Adds the specified nodes to the front of the element list. It returns a pointer to the itself.




.. function:: addToEnd([nodes ...])
	:noindex:

	Adds the specified nodes to the end of the element list. It returns a pointer to the itself.




.. function:: addAfter(node [nodes ...])
	:noindex:

	Adds the specified nodes to the element list after node. It returns a pointer to itself.



.. function:: indexOf(node)
	:noindex:

	Returns the index of pNode in the list of elements.


