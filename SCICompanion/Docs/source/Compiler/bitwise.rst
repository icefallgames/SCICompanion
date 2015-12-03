.. Bitwise operators

.. include:: /includes/standard.rst

===================
 Bitwise operators
===================

Bitwise operators operate in individual bits. You'll often see numbers used in these
operations defined in hex (beginning with $), as it is easier to see the bit layout with hexadecimal numbers.

Bitwise AND: &
===============

Compares two bits and generates a 1 result if both bits are 1, otherwise it returns 0::

	; Result will equal $5:
	(= Result (& $85 $0F))
	; Result will equal $1:
	(= Result (& $FF $0F $01))

Often used to set for things like control colors, since onControl() returns a bit mask::

	(if (& ctlGREEN (gEgo onControl:))
		; Ego is on green.
	)

Bitwise OR: |
==============

Compares two bits and generates a 1 result if either or both bits are 1, otherwise it returns 0::

	; Result will equal $8F:
	(= Result (| $85 $0F))
	; Result will equal $07:
	(= Result (| $01 $02 $04))

Often used to OR two or more flags together

	(theActor signal: (| ignAct fixPriOn))


Bitwise exclusive OR: ^
========================

Compares two bits and generates a 1 result if the bits are complementary, otherwise it returns 0::

	; Result will equal $8A
	(= Result (^ $85 $0F))


Bitwise shift left: <<
========================

Bitwise shift left; moves the bits to the left, it discards the far left bit and assigns 0 to the right most bit::

	; Result will equal $14
	(= Result (<< $85 2))

Bitwise shift right: >>
=========================

Bitwise shift right; moves the bits to the right, discards the far right bit and if unsigned assigns 0 to the left most bit, otherwise sign extends::

	; Result will equal $21
	(= Result (>> $85 2))


Binary NOT: ~
=====================

A binary not. It inverts the bits in an integer::

	(= Result (~ $F00F))	; the result will be $0FF0 (1111000000001111b->0000111111110000b)
	(= Result (~ $78))		; the result will be $FF87 (0000000001111000b->1111111110000111b)
	(= Result (~ 0))		; the result will be $FFFF (0000000000000000->1111111111111111b)
	(= Result (~ $183D))	; the result will be $E7C2 (0001100000111101->1110011111000010b)

