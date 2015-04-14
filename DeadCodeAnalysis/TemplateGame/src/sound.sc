/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 sound.sc
 Contains the sound class, used for all sound/music related needs.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script SOUND_SCRIPT)
/******************************************************************************/
(use "main")
(use "obj")
/******************************************************************************/
(class Sound of Obj
	(properties
		state 0
		number 0
		priority 0
		loop 1
		handle 0
		signal 0
		prevSignal 0
		client 0
		owner 0
	)
	(method (new theOwner)
		(var hObj)
		= hObj (super:new())
		(if(paramTotal)
			(send hObj:
				owner(theOwner)
				yourself()
			)
		)(else
			(send hObj:
				owner(NULL)
				yourself()
			)
		)
	)
	(method (init)
		= signal 0
		= prevSignal 0
		= state 0
		(send gSounds:add(self))
		DoSound(sndINIT self)
	)
	(method (dispose fKEEP_CLIENT)
		(if(paramTotal and (not fKEEP_CLIENT))
			= client NULL
		)
		(send gSounds:delete(self))
		(if(handle)
			DoSound(sndDISPOSE handle)
			= handle NULL
		)
		(super:dispose())
	)
	(method (play theClient)
		(self:stop())
		(if( not loop )
			= loop 1
		)
		(self:init())
		(if(paramTotal)
		    = client theClient
		)(else
		    = client NULL
		)
		DoSound(sndPLAY self)
	)
	(method (playMaybe sendParams)
		(self:
			play(rest sendParams)
		)
		(if(== state 2)
			(self:dispose())
		)
	)
	(method (stop fKEEP_CLIENT)
		(if(paramTotal and (not fKEEP_CLIENT))
			= client NULL
		)
		(if(handle)
			DoSound(sndSTOP handle)
		)
	)
	(method (check)
		(if(signal)
			(if(IsObject(client))
				(send client:cue(self))
			)
			= prevSignal signal
			= signal 0
  		)
	)
	(method (pause pSound)
		DoSound(sndPAUSE pSound)
	)
	(method (changeState)
		DoSound(sndUPDATE self)
	)
	(method (clean anOwner)
		(if((not owner) or (== owner anOwner))
			(self:dispose())
		)
	)
	(method (fade pSound)
		(if(paramTotal and (not pSound))
		    = client 0
		)
		DoSound(sndFADE handle)
	)
)
/******************************************************************************/
