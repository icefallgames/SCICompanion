(version 2)
(include "sci.sh")
(use "Main")
(use "Obj")
(script 989)



(class Sound of Obj
    (properties
        nodePtr 0
        handle 0
        flags $0000
        number 0
        vol 127
        priority 0
        loop 1
        signal $0000
        prevSignal 0
        dataInc 0
        min 0
        sec 0
        frame 0
        client 0
        owner 0
    )

    (method (new param1)
        (send ((super:new())):
            owner(
                (if (paramTotal)
                    param1
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


    (method (stop)
        (if (handle)
            DoSound(sndUPDATE_CUES self)
            DoSound(sndSTOP self)
        )
    )


    (method (pause param1)
        (if (not paramTotal)
            = param1 1
        )
        DoSound(sndPAUSE 
            (if ((self:isMemberOf(Sound)))
                self
            )(else
                0
            )
 			param1)
    )


    (method (hold param1)
        (if (not paramTotal)
            = param1 1
        )
        DoSound(sndSET_HOLD self param1)
    )


    (method (release)
        DoSound(sndSET_HOLD self 0)
    )


    (method (fade theClient param2 param3 param4)
        (var theParamTotal)
        = theParamTotal paramTotal
        (if (paramTotal and IsObject(theClient[(- paramTotal 1)]))
            = theParamTotal (- paramTotal 1)
            = client theClient[theParamTotal]
        )
        (if (theParamTotal)
            DoSound(sndFADE self theClient param2 param3 param4)
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


    (method (setVol param1)
        DoSound(sndSET_VOLUME self param1)
    )


    (method (setPri param1)
        DoSound(sndSET_PRIORITY self param1)
    )


    (method (setLoop param1)
        DoSound(sndSET_LOOP self param1)
    )


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


    (method (clean param1)
        (if (not owner or (== owner param1))
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
