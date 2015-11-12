.. Cycle

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Cycle (of :class:`Obj`)
=======================

.. class:: Cycle

	Defined in Cycle.sc.

	
	The base class for all cyclers. Cyclers are responsible for changing the cel
	of a :class:`Prop` or :class:`Actor` over time, giving the illusion of animation.


Subclasses: :class:`Smopper`, :class:`FlickerCycler`, :class:`Fwd`, :class:`MouthSync`, :class:`Rev`, :class:`Blink`, :class:`CT`, :class:`ROsc`, :class:`Osc`, :class:`RandCycle`, :class:`MCyc`, :class:`Grycler`.

Properties
==========

Defined in Cycle:

========= ======================================================
Property  Description                                           
========= ======================================================
client    The object to which this is attached.                 
caller    The object that is cue()'d when the cycle is complete.
cycleDir  cdFORWARD or cdBACKWARD.                              
cycleCnt                                                        
completed                                                       
========= ======================================================


Methods
==========

.. function:: init(theClient)
	:noindex:



.. function:: nextCel()
	:noindex:



.. function:: cycleDone()
	:noindex:



.. function:: motionCue()
	:noindex:



