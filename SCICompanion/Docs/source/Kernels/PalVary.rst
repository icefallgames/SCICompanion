.. PalVary

.. default - domain::js

.. include:: /includes/standard.rst

================
PalVary (Kernel)
================


	PalVary manages transitions between the current palette and a new palette. It has a number of different subfunctions
	which will be described separately.

	.. IMPORTANT:: SCI1.1 only.

.. function:: PalVary(pvINIT resourceNumber ticks [stepStop direction])

	Begins a transition to a new palette. This only needs to be called once.

	:param number resourceNumber: The palette resource number.
	:param number ticks: The number of ticks between each step in the transition (60 ticks is one second).
	:param number stepStop: How far along the transition to go, ranging from 0-64, where 64 is the complete transition. The default is 64.
	:param number direction: 1 for forward, -1 for reverse.

	Example::

		(PalVary pvINIT targetPalette 1 64 3)

.. function:: PalVary(pvUNINIT)
	:noindex:

	Stops a palette transition. Before performing other palette effects, an existing transition must be stopped.

	Example::

		(method (doit)
			(super doit:)
			(if (== 64 (PalVary pvGET_CURRENT_STEP))
				(PalVary pvUNINIT)
				; The transition is now done...
			)
		)

.. function:: PalVary(pvREVERSE [ticks stepStop direction])
	:noindex:

	Reverses an existing palette transition.

	:param number ticks: The number of ticks between each step in the transition (60 ticks is one second).
	:param number stepStop: How far along the transition to go, ranging from 1-64, where 64 is the complete transition.
	:param number direction: 1 for forward, -1 for reverse.

.. function:: PalVary(pvGET_CURRENT_STEP)
	:noindex:

	Returns the current step of the transition, ranging between 0 and 64. 64 indicates the transition is complete.

	:returns: The current step (between 0 and 64).

.. function:: PalVary(pvCHANGE_TARGET resourceNumber)
	:noindex:

	Changes the target palette of the transition.

	:param number resourceNumber: The palette resource number.

.. function:: PalVary(pvCHANGE_TICKS ticks)
	:noindex:

	Sets the number of ticks between each step of the palette transition.

	:param number ticks: The number of ticks between each step in the transition (60 ticks is one second).

.. function:: PalVary(pvPAUSE_RESUME fPause)
	:noindex:

	Pauses or resumes a palette transition.

	:param boolean fPause: If TRUE, the transition is paused. If FALSE, the transtion is resumed.



