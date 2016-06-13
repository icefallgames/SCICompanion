.. DoSound

.. default - domain::js

.. include:: /includes/standard.rst

================
DoSound (Kernel)
================


	Do sound handles all sound related functions such as playing, loading, unloading, stopping, pausing, etc. It uses the first parameter to identify its subfunction, and the following parameters vary.

.. function:: DoSound(sndINIT soundObj)

	Sets up the specified sound object. It loads the sound resource specified by the sound object's number property and sets it's status property to ssINITIALIZED.

	:param heapPtr soundObj: An instance of :class:`Sound`.

	Example::

		(DoSound sndINIT soundObj)

.. function:: DoSound(sndPLAY soundObj)
	:noindex:

	Plays the sound specified by the given sound object and sets the object's status property to sndPLAYING.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndDISPOSE soundObj)
	:noindex:

	Disposes of the sound specified by the given object unloading it and removing it from the sound system.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndSET_SOUND [fTurnOn])
	:noindex:

	Determines if the sound system is muted or not, and optionally mutes or unmutes it.

	:param boolean fTurnOn: Optional parameter to unmuted or mute the sound. If TRUE, the sound system is unmuted.
	:returns: TRUE if the sound system is unmuted, FALSE otherwise.

	Example::

		; If the sound system is not muted, mute it.
		(if (DoSound sndSET_SOUND)
			(DoSound sndSET_SOUND FALSE)
		)

.. function:: DoSound(sndSTOP soundObj)
	:noindex:

	Stops playing the sound specified by the given sound object and sets the object's status property to sndSTOPPED.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndPAUSE soundObj)
	:noindex:

	Pauses the sound specified by the given sound object and sets the object's status property to ssPAUSED.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndRESUME)
	:noindex:

	.. IMPORTANT:: SCI0 only.

	Resumes playing sounds after a game has been resotred.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndVOLUME [volume])		**SCI0**
	:noindex:

.. function:: DoSound(sndMASTER_VOLUME [volume])		**SCI1.1**
	:noindex:

	Returns the currently set master sound volume, which is a number ranging from 0 to 15. Optionally sets the master volume.

	:param number volume: Optional new volume to set it to.
	:returns: The current volume.

.. function:: DoSound(sndUPDATE soundObj)
	:noindex:

	Tells the sound system to update the specified sound. It notifies it that the sound has changed so its priority and loop properties are re-evaluated.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndFADE soundObj)   SCI0
	:noindex:

	.. IMPORTANT:: SCI0 only.

	Fades the specified sound for roughly two seconds, then stops it, setting it's status property to ssSTOPPED.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndFADE soundObj targetVolume fadeTicker fadeStep stopAfterFade)   SCI1.1
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Fades the sound. This provides more options than the SCI0 version.

	:param heapPtr soundObj: An instance of :class:`Sound`.
	:param number targetVolume: The target volume, ranging from 0 - 127.
	:param number fadeTicker: Unknown (25 is a common value to specify).
	:param number fadeStep: The volume increment to fade each game cycle (10 is a common value).
	:param boolean stopAfterFade: If TRUE, the sound is stopped after the fade is complete.


.. function:: DoSound(sndCHECK_DRIVER)
	:noindex:

	.. IMPORTANT:: SCI0 only.

	Returns the number of voices the sound hardware is capable of playing (0 if no sound driver is installed).

	Example::

		(if (not (DoSound sndCHECK_DRIVER))
			 (Print "The sound driver is not installed!")
		)

.. function:: DoSound(sndSTOP_ALL)
	:noindex:

	.. IMPORTANT:: SCI0 only.

	Stops all the sounds currently playing.

.. function:: DoSound(sndGET_POLYPHONY)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	:returns: The number of voices the sound hardware is capable of playing.

.. function:: DoSound(sndGET_AUDIO_CAPABILITY)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Determines digital audio support.

	:returns: TRUE if digital audio support is available, otherwise FALSE.

.. function:: DoSound(sndSUSPEND)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Unkonwn functionality. Not used in any Sierra games?

.. function:: DoSound(sndSET_HOLD soundObj marker)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Sets the hold spot where the sound should be looped at. This is commonly used
	for engine sounds that want to loop at different spots for different pitches as
	the engine is starting and stopping.

	:param heapPtr soundObj: An instance of :class:`Sound`.
	:param number marker: Index of a hold marker in the Sound resource.

	.. NOTE:: SCI Companion doesn't currently support editing the hold markers in a sound resource.

.. function:: DoSound(UPDATE_CUES soundObj)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Updates the sound.

	:param heapPtr soundObj: An instance of :class:`Sound`.

.. function:: DoSound(sndSEND_MIDI soundObj channel command controller parameter)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Sends MIDI information to the sound resource.

	:param heapPtr soundObj: An instance of :class:`Sound`.
	:param number channel: The MIDI channel (e.g. 0-15).
	:param number command: The MIDI command.
	:param number controller: The MIDI controller.
	:param number parameter: A parameter for the command.

.. function:: DoSound(sndGLOBAL_REVERB reverb)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	:param number reverb: A reverb level (0 - 10).
	:returns: The previous reverb level.



