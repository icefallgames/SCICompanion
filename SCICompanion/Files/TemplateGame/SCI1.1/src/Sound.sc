(version 2)
(include "sci.sh")
(use "Main")
(use "Obj")
(script 989)

/*
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
		(rainSound:play())
		
*/
(class Sound of Obj
    (properties
        nodePtr 0
        handle 0
        flags $0000
        number 0
        vol 127				// 0 to 127.
        priority 0
        loop 1				// -1 for looping.
        signal $0000
        prevSignal 0
        dataInc 0
        min 0
        sec 0
        frame 0
        client 0
        owner 0
    )

    (method (new theOwner)
        (send ((super:new())):
            owner(
                (if (paramTotal)
                    theOwner
                )(else
                    0
                )
				 )
            yourself()
        )
    )


    (method (init)
        = prevSignal (= signal 0)
        (send gSounds:add(self))
        DoSound(sndINIT self)
    )


    (method (dispose)
        (send gSounds:delete(self))
        (if (nodePtr)
            DoSound(sndDISPOSE self)
            = nodePtr 0
        )
        (super:dispose())
    )

	/*
	.. function:: play([theClient])
	
		Plays the sound.
		
		:param heapPtr theClient: An optional object that will be cue()'d when the sound has finished playing, or a cue marker is reached.
	*/
    (method (play theClient)
        (var theParamTotal)
        = theParamTotal paramTotal
        (if (paramTotal and IsObject(theClient[(- paramTotal 1)]))
            = theParamTotal (- paramTotal 1)
            = client theClient[theParamTotal]
        )(else
            = client 0
        )
        (if (not (send gSounds:contains(self)))
	        (self:init())
		)
        (if (not loop)
            = loop 1
        )
        (if (theParamTotal)
            = vol theClient
        )(else
            = vol 127
        )
        DoSound(sndPLAY self 0)
    )

	// Stops the sound.
    (method (stop)
        (if (handle)
            DoSound(sndUPDATE_CUES self)
            DoSound(sndSTOP self)
        )
    )

	/*
	.. function:: pause([fPause])
	
		Pauses or unpauses the sound.
		
		:param boolean fPause: If TRUE or unspecified, the sound is paused. Otherwise it is unpaused.
	*/
    (method (pause fPause)
        (if (not paramTotal)
            = fPause 1
        )
        DoSound(sndPAUSE 
            (if ((self:isMemberOf(Sound)))
                self
            )(else
                0
            )
 			fPause)
    )


    (method (hold fHold)
        (if (not paramTotal)
            = fHold 1
        )
        DoSound(sndSET_HOLD self fHold)
    )


    (method (release)
        DoSound(sndSET_HOLD self 0)
    )

	/*
	.. function:: fade([theClient])
	
	.. function:: fade(toVolume fadeTicker fadeStep stopAfterFade [theClient])
	
		Fades a sound.
		
		:param heapPtr theClient: An optional object that becomes the sound's client. It will be cue()'d when the fade is done.
		:param number toVolume: The target volume (0 if not provided). Ranges from 0-127.
		:param number fadeTicker: Unknown (25 if not provided).
		:param number fadeStep: The volume increment to fade each game cycle (10 if not provided).
		:param boolean stopAfterFade: TRUE if the sound should stop after fading, otherwise FALSE.
	*/
    (method (fade param1 param2 param3 param4)
        (var theParamTotal)
        = theParamTotal paramTotal
        (if (paramTotal and IsObject(param1[(- paramTotal 1)]))
            = theParamTotal (- paramTotal 1)
            = client param1[theParamTotal]
        )
        (if (theParamTotal)
            DoSound(sndFADE self param1 param2 param3 param4)
        )(else
            DoSound(sndFADE self 0 25 10 1)
        )
    )


    (method (mute param1 param2)
        (var temp0)
        (if (not paramTotal)
            = param1 1
        )
        (if (< paramTotal 2)
            = temp0 1
            (while (< temp0 17)
                DoSound(sndSEND_MIDI self temp0 78 param1)
                ++temp0
            )
        )(else
            DoSound(sndSEND_MIDI self param2 78 param1)
        )
    )

	// Sets the sound volume (0 - 127).
    (method (setVol theVolume)
        DoSound(sndSET_VOLUME self theVolume)
    )

	// Sets the sound priority.
    (method (setPri thePri)
        DoSound(sndSET_PRIORITY self thePri)
    )

	/*
		Sets the loop value (-1 to loop endlessly, any other value to play once).
	*/
    (method (setLoop loopValue)
        DoSound(sndSET_LOOP self loopValue)
    )

	// Sends MIDI data to the audio player.
    (method (snd2 param1 param2 param3 param4)
        (if ((<= 1 param1) and (<= param1 15))
            (if (< param2 128)
                DoSound(sndSEND_MIDI self param1 176 param2 param3)
            )(else
                DoSound(sndSEND_MIDI self param1 param2 param3 param4)
            )
        )
    )


    (method (check)
        (if (handle)
            DoSound(sndUPDATE_CUES self)
        )
        (if (signal)
            = prevSignal signal
            = signal 0
            (if (IsObject(client))
                (send client:cue(self))
            )
        )
    )


    (method (clean theOwner)
        (if (not owner or (== owner theOwner))
            (self:dispose())
        )
    )

    (method (playBed theClient)
        (var theParamTotal)
        = theParamTotal paramTotal
        (if (paramTotal and IsObject(theClient[(- paramTotal 1)]))
            = theParamTotal (- paramTotal 1)
            = client theClient[theParamTotal]
        )(else
            = client 0
        )
        (self:init())
        (if (not loop)
            = loop 1
        )
        (if (theParamTotal)
            = vol theClient
        )(else
            = vol 127
        )
        DoSound(sndPLAY self 1)
    )


    (method (changeState)
        DoSound(sndUPDATE self)
    )

)
