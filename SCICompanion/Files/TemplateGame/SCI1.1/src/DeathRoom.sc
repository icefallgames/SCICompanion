/*
	This is a room for handling death scenes. In the sGeneric Script, you can switch based on of the
	gDeathReason global variable to display custom scenes depending on the type of death.
	The death room is triggered by calling the :func:`Die` procedure with a number corresponding to a death reason.
*/
(version 2)
(include "sci.sh")
(include "Verbs.sh")
(include "game.sh")
(include "20.shm")
(exports
    0 deathRoom
)
(use "Main")
(use "Cycle")
(use "Game")
(use "Actor")
(use "Obj")
(script DEATH_SCRIPT)

(local
    messageBuffer[200]
)
(procedure (localproc_006e)
    (iWannaQuit:init())
    (iWannaRestore:init())
    (iWannaRestart:init())
    (send gUser:
        canControl(1)
        canInput(1)
    )
    (send gIconBar:enable())
    (send gIconBar:enable(ICONINDEX_DO))
    (send gIconBar:select((send gIconBar:at(2))))
    (send gGame:setCursor(999))
)

(instance public deathRoom of Rm
    (properties
        picture 200
    )

    (method (init)
        Palette(palSET_INTENSITY 0 255 100)
        PalVary(pvUNINIT)
        (send gGame:handsOff())
        (super:init())
        (send gMusic2:stop())
        (switch (gDeathReason)
        	// TODO: Add special cases here
            (default
                (send gRoom:setScript(sGeneric))
            )
        )
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case V_DO
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )
)

(instance sGeneric of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                Message(msgGET DEATH_SCRIPT N_DEATH 0 0 gDeathReason @messageBuffer)
                Display(@messageBuffer dsCOORD 143 68 dsCOLOR 0 dsBACKGROUND 5 dsFONT 1605 dsWIDTH 140 dsALIGN alCENTER)
                (skull:
                    view(2000)
                    loop(0)
                    cel(0)
                    init()
                    posn(42 101)
                    setCycle(Fwd)
                )
                = seconds 2
            )
            (case 1
                localproc_006e()
                (self:dispose())
            )
        )
    )

)

(instance skull of Prop
    (properties
        signal $4000
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case V_DO
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance iWannaRestart of View
    (properties
        x 50
        y 170
        view 2099
        loop 1
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case V_DO
                (self:cel(1))
                (send gGame:restart())
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance iWannaRestore of View
    (properties
        x 150
        y 170
        view 2099
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case V_DO
                (self:cel(1))
                (send gGame:restore())
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance iWannaQuit of View
    (properties
        x 250
        y 170
        view 2099
        loop 2
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case V_DO
                (self:cel(1))
                = gQuitGame TRUE
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
