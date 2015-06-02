
/******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script 14)
/******************************************************************************/
(use "main")
(use "controls")
(use "obj")
(use "user")
(use "File")

/******************************************************************************/

// Temporary implementation for logging to out. Create a "logs" directory as a peer to "src" to have this work.

(procedure public (LogEvent pEvent)
       (var val[50], hFile)
	   	
	   		= hFile (send (File:new()):
	   					name("logs\\LOG.txt"))
		   
           Format(@val "Ticks: %08d " GetTime())
           (send hFile:writeString(@val))
           
          Format(@val "Type: %d " (send pEvent:type))
           (send hFile:writeString(@val))
           
           Format(@val "Message: %d " (send pEvent:message))
           (send hFile:writeString(@val))

           (send hFile:writeString("\n\r"))


		(send hFile:dispose())
)


(procedure public (LogInt theInt)
       (var val[50], hFile)
	   	
	   		= hFile (send (File:new()):
	   					name("logs\\LOG.txt"))
		   
           Format(@val "Int: %d " theInt)
           (send hFile:writeString(@val))

           (send hFile:writeString("\n\r"))

		(send hFile:dispose())
)

