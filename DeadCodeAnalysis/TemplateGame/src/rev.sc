/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 rev.sc
 Contains a cycle class for reverse animation. 
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script REV_SCRIPT)
/******************************************************************************/
(use "cycle")
/******************************************************************************/
(class Rev of Cycle
	(properties
		client 0
		caller 0
		cycleDir cdBACKWARD
		cycleCnt 0
		completed 0
	)
	(method (doit)
		(var nCel)
		= nCel (self:nextCel)
		(if(< nCel 0)
		    (self:cycleDone())
		)(else
			(send client:cel(nCel))
		)
	)
	(method (cycleDone)
		(send client:cel( (send client:lastCel) ) )
	)
)
/******************************************************************************/
