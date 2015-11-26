.. Class header

.. include:: /includes/standard.rst

================================
 Accessing an object's header
================================

To access an object's header, you can use the object specific keywords.

These are read only and should not be changed. They are as follows:

objectFunctionArea
	Contains the address of the object's function area, containing offsets to the object's methods.

objectInfo
	A number containing the object's info. If this is $8000, then the object is a class. Otherwise, it's an instance.

objectLocal
	Contains a pointer to the object's property variables.

objectName
	Contains a pointer to the object's name string.

objectSize
	A number containing the size of the object structure.

objectSpecies
	A pointer to the object in memory.

objectSuperclass
	A pointer the the object's superclass in memory.

objectTotalProperties
	A number containing the object's total number of properties.

objectType
	The object's type. 1 if it's an instance, 6 if it's a class.


Examples::

	(Printf "This object: %s is %d bytes large, has %d properties and $%04x as it's -info-." objectName objectSize objectTotalProperties objectInfo)

	(Printf
		"%s\n"+
		"object at $%04x of $%04x\n"+
		"Methods at: $%04x\n"+
		"-info-: $%04x\n"+
		"Properties at: $%04x\n"+
		"Size: %d bytes\n"+
		"Total Properties: %d\n"+
		"Segment Type: %x\n"
		objectName
		objectSpecies
		objectSuperclass
		objectFunctionArea
		objectInfo
		objectLocal
		objectSize
		objectTotalProperties
		objectType
	)
