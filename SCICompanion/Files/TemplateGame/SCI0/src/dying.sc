/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 dying.sc
 Contains a public script instance to handle when the ego dies.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script DYING_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "dcicon")
(use "cycle")
(use "obj")
/******************************************************************************/
(instance public DyingScript of Script
	(properties)
	(method (changeState newState)
		(var mbResult, message)
		= state newState
		(if(== state 0)
			ProgramControl()
			(send gTheMusic:fade())
			(send gRoom:setScript(0))
			Load(rsSOUND 2)
			= seconds 3
		)(else
  			(if(== state 1)
  				(send gTheSoundFX:stop())
  				(send gTheMusic:
  					number(2)
  					loop(1)
  					priority(-1)
  					play()
  				)
      
                // The following lines give a typical Sierra style
                // message box telling the player that they have died.
                // You can customize it to your liking.

  				(if(<> NULL caller)
  					Load(rsVIEW caller)
  					(deadIcon:view(caller))
  				)(else
  					Load(rsVIEW DYING_SCRIPT)
  					(deadIcon:view(DYING_SCRIPT))
  				)
  				(if(<> NULL register)
  					= message register
  				)(else
  					= message "You are dead."
  				)
  				(if(Print(
  					message
  					#font gDeadFont
  					#icon deadIcon
  					#button "Keep On Muddling" 0
  					#button "Order A Hintbook" 1
  				))
  					Print(
  						"Order a hint book? Who do you think I "+
  						"am? Sierra On-Line? Naw, I'm just a "+
  						"measly computer programmer that made this "+
  						"game in his basement!"
  					)
  				)

  				(while(1)
  					= mbResult Print(
  						"Remember:\nsave early, save often!"
  						#title "Brian Provinciano says:"
  						#font gDeadFont
  						#button "Restore"  1
  						#button "Restart"  2
  						#button "  Quit  " 3
  					)
  					(switch(mbResult)
  						(case 1
  							(if(<> (send gGame:restore()) -1) 
  								return
  							)
		    			)
		    			(case 2
  							(send gGame:restart())
  							return
		    			)
		    			(case 3
		    				= gQuitGame TRUE
		    				return
		    			)
		    		)
		    	)
  			)
  		)
	)
)
/******************************************************************************/
(instance deadIcon of DCIcon
	(properties)
	(method (init)
		(super:init())
		(if(== gRoomNumberExit 540)
			= cycler (End:new())
			(send cycler:init(self))
		)
	)
)
/******************************************************************************/
