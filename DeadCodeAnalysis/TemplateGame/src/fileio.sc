/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 fileio.sc
 Contains a File I/O class to simplify the use of files and rid the need to 
 use kernel functions.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script FILEIO_SCRIPT)
/******************************************************************************/
(use "obj")
/******************************************************************************/
(class File of Obj
	(properties
		handle 0
	)
	(method (dispose)
		(self:close())
		(super:dispose())
	)
	(method (showStr strBuf)
		Format(strBuf "File: %s" objectName)
	)
	(method (open mode)
		(switch(paramTotal)
			(case 0
				= handle FOpen(objectName fOPENFAIL)
		    )
		    (case 1
		    	= handle FOpen(objectName mode)
		    )
		    (default
		    	= handle NULL
		    )
		)
		(if(== handle -1)
		    = handle NULL
		)
		(if(handle)
			return(self)
		)
		return(NULL)
	)
	(method (write putStrings)
		(var i)
		(if(not handle)
			= handle (self:open())
		)
		(if(handle)
  			(for (= i 0) (< i paramTotal) (++i)
  				FPuts(handle putStrings[i])
		    )
		)
	)
	(method (read strBuf size)
		(if(<> paramTotal 2)
			return(0)
		)
		(if(not handle)
		    = handle (self:open(fOPENCREATE))
		)
		(if(handle)
		    return(FGets(strBuf size))
		)
		return(0)
	)
	(method (close)
		(if(handle)
			FClose(handle)
		)
		= handle NULL
	)
)
/******************************************************************************/
