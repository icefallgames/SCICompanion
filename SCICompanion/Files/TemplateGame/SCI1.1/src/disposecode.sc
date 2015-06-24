(version 2)
(include "sci.sh")
(include "game.sh")
(exports
    0 disposeCode
)
(use "Main")
(use "DisposeLoad")
(use "Obj")
(script DISPOSECODE_SCRIPT)

// A place to put script numbers that are rarely used and should be unloaded
// on each room change to free up heap.

(instance public disposeCode of Code
    (properties)

    (method (doit)
        DisposeLoad(0
        	// Insert talker scripts here...
        	ANIMDIALOG_SCRIPT
        	DOOR_SCRIPT
        	INSET_SCRIPT
        	FILE_SCRIPT
        	MESSAGEOBJ_SCRIPT
        	SLIDER_SCRIPT
        	SCALETO_SRIPT
        	PATHAVOIDER_SCRIPT
        	MOVEFORWARD_SCRIPT
        	OSCILLATE_SCRIPT
        	FORWARDCOUNT_SCRIPT
        	MANUALCYCLE_SCRIPT
        	REVERSECYCLE_SCRIPT
        	DIRECTPATH_SCRIPT
        	VELOCITYMOVER_SCRIPT
        	PRITALKER_SCRIPT
        	PATHFOLLOW_SRIPT
        	FEATUREWRITER_SCRIPT)
        	
        (send gOldMH:delete(ScriptID(INGAME_DEBUG_SCRIPT)))
        DisposeScript(GAMECONTROLS_SCRIPT)
        DisposeScript(CONTROLSBASE_SCRIPT)
        DisposeScript(INGAME_DEBUG_SCRIPT)
        DisposeScript(DIRECTPATH_SCRIPT)
        DisposeScript(DISPOSECODE_SCRIPT)	// Ourselves!
    )

)
