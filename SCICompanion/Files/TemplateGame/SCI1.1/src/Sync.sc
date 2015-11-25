(version 2)
(include "sci.sh")
(use "Main")
(use "Timer")
(use "Cycle")
(use "Object")
(script 929)


/*
	This class is involved with lip-syncing and is used internally by and :class:`MouthSync`.
*/
(class Sync of Object
    (properties
        syncTime -1
        syncCue $ffff
        prevCue $ffff
        syncNum -1
    )

    (method (syncStart modNum noun verb cond seq)
        DoSync(syncSTART self modNum noun verb cond seq)
        (if (<> syncCue -1)
            = prevCue syncCue
            = syncTime 0
        )
    )


    (method (syncCheck)
        (if ((<> syncCue -1) and ((<=u syncTime gSyncBias) or (<= syncTime DoAudio(audPOSITION))))
            (if (== (& $fff0 syncCue) 0)
                = prevCue (| (& prevCue $fff0) syncCue)
            )(else
                = prevCue syncCue
            )
            DoSync(syncNEXT self)
        )
    )


    (method (syncStop)
        = prevCue -1
        DoSync(syncSTOP)
    )
)

/*
	This is a cycling class used by :class:`Talker`. It is driven off lip-sync information, and chooses
	the correct mouth cel for the Talker.
*/
(class MouthSync of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
    )

    (method (init theClient modNum noun verb cond seq)
        (super:init(theClient))
        (if (IsObject(gTheSync))
            (send gTheSync:
                syncStop()
                dispose()
            )
        )
        = gTheSync (Sync:new())
        (send gTheSync:syncStart(modNum noun verb cond seq))
    )


    (method (doit)
        (var temp0, gNewSyncSyncTime_2, gNewSyncSyncTime, temp3)
        (super:doit())
        (if (<> (send gTheSync:prevCue) -1)
            = gNewSyncSyncTime (send gTheSync:syncTime)
            = temp3 0
            (while (TRUE)
                = gNewSyncSyncTime_2 (send gTheSync:syncTime)
                (send gTheSync:syncCheck())
                (if (== gNewSyncSyncTime_2 (send gTheSync:syncTime))
                    break
                )
            )
            (if ((<> gNewSyncSyncTime (send gTheSync:syncTime)) and (<> (send client:cel) (= temp0 (& $000f (send gTheSync:prevCue)))))
                (send client:cel(temp0))
            )
        )(else
            = completed 1
            (self:cycleDone())
        )
    )


    (method (dispose)
        (super:dispose())
        (if (gTheSync)
            (send gTheSync:dispose())
            = gTheSync 0
        )
    )


    (method (cue)
        (if (gTheSync)
            (send gTheSync:
                syncStop()
                dispose()
            )
            = gTheSync 0
            (if (caller)
                (send caller:cue())
                = caller 0
            )
        )
    )

)
