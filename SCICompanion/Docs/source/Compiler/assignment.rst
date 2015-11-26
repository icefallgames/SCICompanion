.. Assignment operators

.. include:: /includes/standard.rst

=======================
 Assignment operators
=======================

Use the assignment operators assign values to variables or properties.

The simple assignment operator is =. It assigns the result of code block to variable::

	(= variable code block)

The complex assignment operators are +=, -=, \*=, /=, mod=, &=, \|=, ^=, >>= and <<=. These operators . These operators use the result of code block and apply the variable to it using the specified operator::

	(operator variable code block)

Example::

	(= SomeVar 10)
	(+= SomeVar 40) // SomeVar will equal 50

