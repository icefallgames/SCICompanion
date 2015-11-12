.. Scaler

.. default - domain::js

.. include:: /includes/standard.rst

=========================
Scaler (of :class:`Code`)
=========================

.. class:: Scaler

	Defined in Scaler.sc.

	
	Scaler lets you scale down the size of a view as it recedes into the distance.
	
	It is generally used as part of a setScale call on a :class:`Prop` or one of its subclasses::
	
		(send gEgo:setScale(Scaler frontSize backSize frontY backY))
	


Subclasses: :class:`ScaleTo`.

Properties
==========

Defined in Scaler:

========= ===========
Property  Description
========= ===========
client               
frontY               
backY                
frontSize            
backSize             
slopeNum             
slopeDen             
const                
========= ===========


Methods
==========

.. function:: init(theClient theFrontSize theBackSize theFrontY theBackY)
	:noindex:

	
	:param heapPtr theClient: The object on which to apply the Scaler.
	:param number theFrontSize: The scale of the object (in percentage) when it's at theFrontY.
	:param number theBackSize: The scale of the object (in percentage) when it's at theBackY.
	:param number theFrontY: The largest (closest) y coordinate for the Scaler.
	:param number theBackY: The smallest (furthest) y coordinate for the Scaler.
	


.. function:: doit()
	:noindex:



