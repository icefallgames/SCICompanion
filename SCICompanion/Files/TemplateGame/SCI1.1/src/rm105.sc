(version 2)
(include "sci.sh")
(include "game.sh")
(include "105.shm")
(use "Controls")
(use "Cycle")
(use "Door")
(use "Feature")
(use "Game")
(use "Main")
(use "Obj")
(script 105)

(instance public rm105 of Rm
    (properties
        picture 31
        north 110
        horizon 50
        east 0
        south 0
        west 0
        noun N_ROOM
    )

    (method (init)
        (super:init())
        (self:setScript(RoomScript))
        (switch (gPreviousRoomNumber)
            (default 
                (send gEgo:
                    posn(150 100)
                    loop(1)
                )
            )
        )
        (send gEgo:init())
    )

)
(instance RoomScript of Script
    (properties)

    (method (doit)
        (super:doit())
        // code executed each game cycle
    )


    (method (handleEvent pEvent)
        (super:handleEvent(pEvent))
        // handle Said's, etc...
    )


    (method (changeState newState)
        = state newState
        (switch (state)
            (case 0
                // Handle state changes
            )
        )
    )

)
