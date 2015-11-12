.. Relational operators

.. include:: /includes/standard.rst

================================
 Relational operators
================================

Use the arithmetic operators to compare to values to each other. These can be used in :doc:`conditionalexpressions` or just in general expressions.

Equal: ==
============

Returns the TRUE if A is equal to B, otherwise, returns FALSE.

Example::

	(if (== someNumber 5)
		// Do something if someNumber is 5
	)


Not equal: <>
===============

Returns the TRUE if A is not equal to B, otherwise, returns FALSE.

Example::

	(if (<> someNumber 5)
		// Do something if someNumber is not 5
	)	

Less than and greater than operators
=====================================

=========== =====================================================================
Operator    Description
=========== =====================================================================
<           TRUE if A is less than B
<u          TRUE if A is less than B, where A and B are unsigned
<=          TRUE if A is less than or equal to B
<=u         TRUE if A is less than or equal to B, where A and B are unsigned
>           TRUE if A is greater than B
>u          TRUE if A is greater than B, where A and B are unsigned
>=          TRUE if A is greater than or equal to B
>=u         TRUE if A is greater than or equal to B, where A and B are unsigned
=========== =====================================================================

Example::

	(if (< 5 10))
		// This branch will be taken since 5 is less than 10.
	)
	(else
		// This branch won't.
	)

	(if (>= 10 10)
		// This branch will be taken, since 10 is greater than or equal to 10.
	)
	(else
		// This won't.
	)

The unsigned operators are useful if you are dealing with large numbers. By default, SCI treats numbers
as signed. That means the range of a 16-bit value is [-32768, 32767]. If you use values larger than
32767, they will be treated as negative unless you use unsigned comparison operators. In that case, the
values are treated as though they are in the range [0, 65535]::

	(= a 40000) // This is -25536 when treated as a 16-bit signed number.

	// Now ask if a is greater than zero
	(if (> a 0)
		// This branch is not taken, because a is treated as -25536.
	)

	// Use an unsigned comparison to treat a as unsigned.
	(if (>u a 0)
		// This branch is taken, because 40000 is bigger than zero.
	)
