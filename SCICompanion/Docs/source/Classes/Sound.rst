.. Sound

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Sound (of :class:`Object`)
==========================

.. class:: Sound

	Defined in Sound.sc.

	
	The Sound class encapsulates sound functionality.
	Sound (midi) and audio (digial audio) resources share the same numbering namespace.
	That means that if you have both, sound 47 should be a midi version of audio 47, and
	audio 47 should be a digital audio version of sound 47. The appropriate one will play
	depending on the user's current configuration (resource.cfg in the game folder).
	
	Example usage::
	
		(instance rainSound of Sound
			(properties
				number 567	// Sound resource 567
				loop -1
			)
		)
	
		// Then in script at some point:
		(rainSound play:)
	


Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Sound:

========== ===============
Property   Description    
========== ===============
nodePtr                   
handle                    
flags                     
number                    
vol        0 to 127.      
priority                  
loop       -1 for looping.
signal                    
prevSignal                
dataInc                   
min                       
sec                       
frame                     
client                    
owner                     
========== ===============


Methods
==========

.. function:: new(theOwner)
	:noindex:



.. function:: init()
	:noindex:



.. function:: dispose()
	:noindex:




.. function:: play([theClient])
	:noindex:

	Plays the sound.

	:param heapPtr theClient: An optional object that will be cue()'d when the sound has finished playing, or a cue marker is reached.



.. function:: stop()
	:noindex:

	Stops the sound.



.. function:: pause([fPause])
	:noindex:

	Pauses or unpauses the sound.

	:param boolean fPause: If TRUE or unspecified, the sound is paused. Otherwise it is unpaused.



.. function:: hold(fHold)
	:noindex:



.. function:: release()
	:noindex:




.. function:: fade([theClient])
	:noindex:

.. function:: fade(toVolume fadeTicker fadeStep stopAfterFade [theClient])
	:noindex:

	Fades a sound.

	:param heapPtr theClient: An optional object that becomes the sound's client. It will be cue()'d when the fade is done.
	:param number toVolume: The target volume (0 if not provided). Ranges from 0-127.
	:param number fadeTicker: Unknown (25 if not provided).
	:param number fadeStep: The volume increment to fade each game cycle (10 if not provided).
	:param boolean stopAfterFade: TRUE if the sound should stop after fading, otherwise FALSE.



.. function:: mute(param1 param2)
	:noindex:



.. function:: setVol(theVolume)
	:noindex:

	Sets the sound volume (0 - 127).


.. function:: setPri(thePri)
	:noindex:

	Sets the sound priority.


.. function:: setLoop(loopValue)
	:noindex:

	
	Sets the loop value (-1 to loop endlessly, any other value to play once).
	


.. function:: snd2(param1 param2 param3 param4)
	:noindex:

	Sends MIDI data to the audio player.


.. function:: check()
	:noindex:



.. function:: clean(theOwner)
	:noindex:



.. function:: playBed(theClient)
	:noindex:



.. function:: changeState()
	:noindex:



