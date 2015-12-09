.. defines and enums

.. include:: /includes/standard.rst

=================
 define and enum
=================

The **define** keyword allows you to assign new names to words. You can use the define keyword on nearly any word.

Examples::

	(define rsVIEW $80) ; replaces any occurance of rsVIEW with $80 at compile time 
	(define PIC_OPEN_FROM_RIGHT 2)

The **enum** keyword lets you define symbols for sequentially increasing values. For instance, in the following example,
DOOR_OPEN will be equal to 0, DOOR_OPENING will be equal to 1, and so on::

	; Some door states
	(enum
		DOOR_OPEN
		DOOR_OPENING
		DOOR_CLOSED
		DOOR_CLOSING
	)

You can also specify a starting number for the enum values::

	(enum 8
		DOOR_OPEN		; this will be 8
		DOOR_OPENING	; this will be 9
		DOOR_CLOSED		; this will be 10, and so on...
	)