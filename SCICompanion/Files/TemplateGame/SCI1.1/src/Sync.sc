(version 2)
(include "sci.sh")
(use "Main")
(use "Timer")
(use "Cycle")
(use "Obj")
(script 929)



(class Sync of Obj
    (properties
        syncTime -1
        syncCue $ffff
        prevCue $ffff
        syncNum -1
    )

    (method (syncStart param1 param2 param3 param4 param5)
        DoSync(0 self param1 param2 param3 param4 param5)
        (if (<> syncCue -1)
            = prevCue syncCue
            = syncTime 0
        )
    )


    (method (syncCheck)
        (if ((<> syncCue -1) and ((<=u syncTime global81) or (<= syncTime DoAudio(6))))
            (if (== (& $fff0 syncCue) 0)
                = prevCue (| (& prevCue $fff0) syncCue)
            )(else
                = prevCue syncCue
            )
            DoSync(1 self)
        )
    )


    (method (syncStop)
        = prevCue -1
        DoSync(2)
    )

)
(class ScriptSync of Obj
    (properties
        prevSignal -1
        playing 0
    )

    (method (init param1 param2 param3 param4 param5)
        (if (gNewSync)
            (self:cue())
        )
        = gNewSync (Sync:new())
        (send gNewSync:
            init()
            syncStart(param1 param2 param3 param4 param5)
        )
        (if (<> (send gNewSync:prevCue) -1)
            = playing 1
            (send gRegions:add(self))
        )
        (Timer:setTicks(self DoAudio(1 param1 param2 param3 param4 param5)))
    )


    (method (doit)
        (var gNewSyncSyncTime)
        (if (<> (send gNewSync:prevCue) -1)
            (while (TRUE)
                (if (== (send gNewSync:prevCue) -1)
                    break
                )
                = gNewSyncSyncTime (send gNewSync:syncTime)
                (send gNewSync:syncCheck())
                (if (== gNewSyncSyncTime (send gNewSync:syncTime))
                    break
                )
            )
            = prevSignal (send gNewSync:prevCue)
        )
    )


    (method (cue)
        Animate((send gOldCast:elements) 0)
        = playing 0
        = prevSignal 32767
        (send gRegions:delete(self))
        (if (gNewSync)
            (send gNewSync:
                syncStop()
                dispose()
            )
            = gNewSync 0
        )
    )

)
(class MouthSync of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
    )

    (method (init param1 param2 param3 param4 param5 param6)
        (super:init(param1))
        (if (IsObject(gNewSync))
            (send gNewSync:
                syncStop()
                dispose()
            )
        )
        = gNewSync (Sync:new())
        (send gNewSync:syncStart(param2 param3 param4 param5 param6))
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
