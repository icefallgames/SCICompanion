/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 disposeload.sc
 Contains a procedure for disposing/loading resources.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script DISPOSELOAD_SCRIPT)
/******************************************************************************/
(procedure public (DisposeLoad rsType rsNumbers)
	(var i, num)
	= paramTotal (- paramTotal 2)
	(for (= i 0) (<= i paramTotal) (++i)
		= num rsNumbers[i]
		(if(rsType)
			Load(rsType num)
		)(else
			DisposeScript(num)
		)
	)
)
/******************************************************************************/
