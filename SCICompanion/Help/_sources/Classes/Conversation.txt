.. Conversation

.. default - domain::js

.. include:: /includes/standard.rst

===============================
Conversation (of :class:`List`)
===============================

.. class:: Conversation

	Defined in MessageObj.sc.

	
	This seems to be a class to which you can add a series of messages that will be displayed one
	after another. :class:`Messager` does this automatically, but only if messages have the same noun/verb/cond tuple
	and sequentially increasing sequence numbers.


Properties
==========

Inherited from :class:`List`:

======== ===========
Property Description
======== ===========
elements            
size                
======== ===========

Defined in Conversation:

======== ===========
Property Description
======== ===========
script              
curItem             
caller              
======== ===========


Methods
==========

.. function:: init(theCaller)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:




.. function:: add([moduleNumber noun verb condition sequence x y font)
	:noindex:

	Adds a new message to the conversation.

	:param number moduleNumber: Room number, or -1 for the current room.
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence.
	:param number x: The message x position.
	:param number y: The message y position.
	:param number font: The message font.



.. function:: cue(param1)
	:noindex:



.. function:: setScript(theScript sendParams)
	:noindex:



.. function:: load(param1)
	:noindex:



