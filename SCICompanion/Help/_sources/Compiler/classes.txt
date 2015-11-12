.. Classes

.. include:: /includes/standard.rst

=======================
Classes and instances
=======================

SCI games are object oriented. The objects in SCI are called "classes" and "instances".

Classes can be created from scratch, or based on other classes. Instances, however, must be derived from a class.
You can think of a class as the blueprint for creating a building, and an instance as a building itself.

Classes can be accessed globally, while instances can only be directly accessed locally (within the same script).
To globally access instances, you would have to store it's address in a global variable, and reference it from the variable.

Classes and instances have two parts: properties and methods.

Properties are 16 bit integers. They work just like other variables, but are attatched to objects.

Methods are functions containing code which can be executed by the interpreter. They are declared just like :doc:`procedures`.

Of note, instances can declare new methods on themselves. But they cannot delcare new properties. An instance may only have the 
properties that were defined on the Class from which it is derived. Though instances cannot add properties, they can change the
default values of properties.

To access the header of a class or instance, you can use object specific :doc:`keywords <classheader>`.



Class syntax::

	(class <ClassName> [of <SuperClassName> (optional)]
		(properties
			<PropertyName> <PropertyValue>
			...
		)
		(method (<MethodName> [<ParamName1> <ParamName2> ... (optional)])
			<code>
		)
	)

Instance syntax::

	(instance [public] <InstanceName> of <SuperClassName>
		(properties
			<PropertyName> <PropertyValue>
			...
		)
		(method (<MethodName> [<ParamName1> <ParamName2> ... (optional)])
			<code>
		)
	)	

Examples::

	(class Ego 
		(properties 
			x 0
			y 0
			view 0
			loop 0
			cel 0
			priority 0
		) 
		(method (draw) 
			DrawCel(view loop cel x y priority) 
		) 
		(method (setCoords newX newY) 
			= x newX 
			= y newY
		)
	)

	(class LarryClass of Ego 
		(properties 
			x 0 
			y 0 
			view 0 
			loop 0 
			cel 0 
			priority 0 
			anotherProperty 200
		) 
		(method (getCelWidth) 
			return(CelWide(view loop cel)) 
		) 
	)

	(instance public Larry of LarryClass 
		(properties 
			x 80 
			y 60 
			view 500 
			loop 0 
			cel 0 
			priority 0 
			anotherProperty 9900 
		)
	)