(version 2)
(include "sci.sh")
(use "Main")
(use "Timer")
(use "Cycle")
(use "Obj")
(script 929)


/*
	This class is involved with lip-syncing and is used internally by and :class:`MouthSync`.
*/
(class Sync of Obj
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
        (if ((<> syncCue -1) and ((<=u syncTime global81) or (<= syncTime DoAudio(audPOSITION))))
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
        (if (IsObject(gNewSync))
            (send gNewSync:
                syncStop()
                dispose()
            )
        )
        = gNewSync (Sync:new())
        (send gNewSync:syncStart(modNum noun verb cond seq))
    )


    (method (doit)
        (var temp0, gNewSyncSyncTime_2, gNewSyncSyncTime, temp3)
        (super:doit())
        (if (<> (send gNewSync:prevCue) -1)
            = gNewSyncSyncTime (send gNewSync:syncTime)
            = temp3 0
            (while (TRUE)
                = gNewSyncSyncTime_2 (send gNewSync:syncTime)
                (send gNewSync:syncCheck())
                (if (== gNewSyncSyncTime_2 (send gNewSync:syncTime))
                    break
                )
            )
            (if ((<> gNewSyncSyncTime (send gNewSync:syncTime)) and (<> (send client:cel) (= temp0 (& $000f (send gNewSync:prevCue)))))
                (send client:cel(temp0))
            )
        )(else
            = completed 1
            (self:cycleDone())
        )
    )


    (method (dispose)
        (super:dispose())
        (if (gNewSync)
            (send gNewSync:dispose())
            = gNewSync 0
        )
    )


    (method (cue)
        (if (gNewSync)
            (send gNewSync:
                syncStop()
                dispose()
            )
            = gNewSync 0
            (if (caller)
                (send caller:cue())
                = caller 0
            )
        )
    )

)
