.. Message

.. default - domain::js

.. include:: /includes/standard.rst

================
Message (Kernel)
================


	Retrieves message resource information. Messages are what is spoken by in-game characters. See :doc:`/messages`.

	.. IMPORTANT:: SCI1.1 only.

.. function:: Message(msgGET moduleNumber noun verb condition sequence textBuffer)

	Gets the text for the specified message resource, and returns the talker number.

	:param number moduleNumber: The module number (room number).
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence number.
	:param string textBuffer: The buffer that receives the message text.
	:returns: A talker number, or 0 if it failed.

	Example::

		(procedure (SomeFunc &tmp [buffer 100])
			(Message msgGET 0 12 0 0 1 @buffer)
			...
		)

.. function:: Message(msgNEXT textBuffer)
	:noindex:

	Gets the text for the message resource that follows the one previously obtained by msgGET. The following message
	is the one with the same moduleNumber/noun/verb/condition tuple, but an incremented sequence number. If no such
	message exists, then the search continues at the message given by the last stack frame.

	:param string textBuffer: The buffer that receives the message text.
	:returns: A talker number, or 0 if it failed.

.. function:: Message(msgLAST_MESSAGE outBuffer)
	:noindex:

	Gets the tuple of the last returned message. This consists of 5 16-bit values:
	moduleNumber, noun, verb, condition and sequence.

	:param heapPtr outBuffer: A buffer large enough to hold 12 bytes (2 for the null terminator)

	Example::

		(procedure (SomeFunc &tmp buffer modNum noun verb condition case)
			(= buffer (Memory memALLOC_CRIT 12))
			(Message msgLAST_MESSAGE buffer)
			(= modNum (WordAt buffer 0))
			(= noun (WordAt buffer 1))
			(= verb (WordAt buffer 2))
			(= condition (WordAt buffer 3))
			(= case (WordAt buffer 4))
			(DoAudio audPLAY modNum noun verb condition case)
		)

.. function:: Message(msgSIZE moduleNumber noun verb condition sequence)
	:noindex:

	Gets the size needed to store the message text (including terminating null).

	:param number moduleNumber: The module number (room number).
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence number.
	:returns: The size in bytes needed to store the message text.

	Example::

		(procedure (SomeFunc modNum noun verb condition seq &tmp theSize title)
			(= theSize (Message msgSIZE modNum noun verb condition seq))
			(if (theSize)
				(= title (Memory memALLOC_CRIT theSize))
				(Message msgGET modNum noun verb condition seq title)
			)
		)

.. function:: Message(msgREF_NOUN moduleNumber noun verb condition sequence)
	:noindex:

.. function:: Message(msgREF_VERB moduleNumber noun verb condition sequence)
	:noindex:

.. function:: Message(msgREF_COND moduleNumber noun verb condition sequence)
	:noindex:

.. function:: Message(msgPUSH)
	:noindex:

.. function:: Message(msgPOP)
	:noindex:


