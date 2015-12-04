/*
	A place to put script numbers that are rarely used and should be unloaded
	on each room change to free up heap space. If you add a new script with a motion class, or a cycler,
	you should probably add your script number here.
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(exports
    0 disposeCode
)
(use "Main")
(use "DisposeLoad")
(use "System")
(script DISPOSECODE_SCRIPT)

(instance public disposeCode of Code
    (properties)

    (method (doit)
        DisposeLoad(0
        	// Insert talker scripts here...
        	CHOICETALKER_SCRIPT
        	DOOR_SCRIPT
        	INSET_SCRIPT
        	FILE_SCRIPT
        	CONVERSATION_SCRIPT
        	SLIDER_SCRIPT
        	SCALETO_SRIPT
        	PATHAVOIDER_SCRIPT
        	MOVEFORWARD_SCRIPT
        	OSCILLATE_SCRIPT
        	FORWARDCOUNT_SCRIPT
        	MOVECYCLE_SCRIPT
        	REVERSECYCLE_SCRIPT
        	DIRECTPATH_SCRIPT
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
