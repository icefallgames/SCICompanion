.. DoAudio

.. default - domain::js

.. include:: /includes/standard.rst

================
DoAudio (Kernel)
================


	DoAudio manages playback of audio resources. The various subfunctions will be discussed separately.

	.. IMPORTANT::
		SCI1.1 only.

.. function:: DoAudio(audPLAY resourceNumber)

.. function:: DoAudio(audPLAY moduleNumber noun verb condition sequence)
	:noindex:

	Starts a digital audio sample. Variations are provided for both
	audio resources (described by a single resource number) and audio36 resources
	(described by a module number and a noun/verb/condition/sequence tuple. The latter
	are used for speech samples associated with message resources.

	:param number resourceNumber: The resource number.
	:param number moduleNumber: The module number (room number).
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence number.
	:returns: The sample length in ticks.

	Example::

		(DoAudio audPLAY 107)

.. function:: DoAudio(audWPLAY resourceNumber)
	:noindex:

.. function:: DoAudio(audWPLAY moduleNumber noun verb condition sequence)
	:noindex:

	Loads a digital sample but does not play it. It returns the length of the sample
	in ticks. This can be useful if some game logic needs to be aware of the length of the audio.
	
	Variations are provided for both
	audio resources (described by a single resource number) and audio36 resources
	(described by a module number and a noun/verb/condition/sequence tuple. The latter
	are used for speech samples associated with message resources.

	:param number resourceNumber: The resource number.
	:param number moduleNumber: The module number (room number).
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence number.
	:returns: The sample length in ticks.

	Example::

		(Timer setTicks: self (DoAudio audWPLAY gRoomNumber noun verb condition seq))

.. function:: DoAudio(audSTOP)
	:noindex:

	Stops the currently playing digital audio sample.

.. function:: DoAudio(audPAUSE)
	:noindex:

	Pauses the currently playing digital audio sample.

.. function:: DoAudio(audRESUME)
	:noindex:

	Resumes the currently playing digital audio sample.

.. function:: DoAudio(audPOSITION)
	:noindex:

	Returns the position of the currently playing digital audio sample.

.. function:: DoAudio(audRATE rate)
	:noindex:
	
	Sets the audio rate.
	
.. function:: DoAudio(audVOLUME volume)
	:noindex:

	Sets the volume of the currently playing audio sample.

	:param number volume: A value between 0 and 127.

.. function:: DoAudio(audLANGUAGE languageId)
	:noindex:

	Sets the audio language.

.. function:: DoAudio(audCD [params...])
	:noindex:

	Controls the CD-audio functions.



