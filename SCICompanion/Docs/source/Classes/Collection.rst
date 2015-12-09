.. Collection

.. default - domain::js

.. include:: /includes/standard.rst

===============================
Collection (of :class:`Object`)
===============================

.. class:: Collection

	Defined in System.sc.

	The Collection class is used to handle a collection of elements, such as objects.


Subclasses: :class:`List`.

Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Collection:

======== ===========
Property Description
======== ===========
elements            
size                
======== ===========


Methods
==========


.. function:: doit([...])
	:noindex:

	Calls the doit method on each element, forwarding any parameters given.



.. function:: dispose()
	:noindex:



.. function:: showStr(buffer)
	:noindex:

	Fills buffer with a text stating the object name and collection size.


.. function:: showSelf()
	:noindex:

	For the object and each of its elements, this prints the object's name and collection size on the screen in a dialog box.



.. function:: add([node ...])
	:noindex:

	Adds the given nodes to the collection of elements. It returns a pointer to itself.
	If no nodes are supplied, it is simply ensured that the list is initialized.

	:param node: An object to add to the collection.
	:returns: Itself.




.. function:: delete([node ...])
	:noindex:

	Deletes the specified nodes from the collection of elements. It returns a pointer to the object.

	:param node: An object to delete from the collection.
	:returns: Itself.




.. function:: eachElementDo(aSelector [...])
	:noindex:

	For each element, this calls aSelector (a method or property), forwarding any other given parameters to it.

	:param selector aSelector: A selector (e.g. #doit, or #moveSpeed).

	Example usage::

		(myPolyList eachElementDo: #perform drawPoly)




.. function:: firstTrue(aSelector [...])	
	:noindex:

	For each element, this calls aSelector (a method or property), and if given, the rest of the parameters.
	The first send to an element to return TRUE will cause firstTrue to return a pointer to that object.
	If no send returns TRUE, firstTrue will return NULL.

	:param selector aSelector: A selector (e.g. #doit, or #moveSpeed).
	:returns: The first object that returned TRUE, or NULL if no objects returned TRUE.





.. function:: allTrue(aSelector [...])	
	:noindex:

	For each element, this calls aSelector (a method or property), and if given, the rest of the parameters.
	If any send to an element returns FALSE, allTrue will return FALSE as well. Otherwise, if all sends have returned TRUE, so will allTrue.	

	:param selector aSelector: A selector (e.g. #doit, or #moveSpeed).
	:returns: TRUE if all objects returned TRUE, otherwise FALSE.



.. function:: contains(node)
	:noindex:

	:returns: TRUE if the object contains node in its collection of elements.


.. function:: isEmpty()
	:noindex:

	:returns: TRUE if the collection contains no nodes (elements). Otherwise, it returns FALSE.


.. function:: first()
	:noindex:

	:returns: a pointer to the first node in the collection.


.. function:: next(node)
	:noindex:

	:returns: a pointer to the node after the specified one.


.. function:: release()
	:noindex:

	Deletes all the nodes (elements) from the collection.


.. function:: isDuplicate()
	:noindex:



