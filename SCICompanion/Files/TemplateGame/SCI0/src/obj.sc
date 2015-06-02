/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 obj.sc
 The base script for all the classes. This contains many of the important 
 classes, including Obj and Event.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script OBJ_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
/******************************************************************************/
(class Obj
	(method (new)
		return(Clone(self))
	)
	(method (init))
	(method (doit)
		return(self)
	)
	(method (dispose)
		DisposeClone(self)
	)
	(method (showStr strBuf)
		StrCpy(strBuf objectName)
	)
	(method (showSelf)
		(var strBuf[200])
		Print( (self:showStr(@strBuf)) )
	)
	(method (perform pObj params)
		(send pObj:
			doit(self rest params)
		)
	)
	(method (isKindOf pObj)
		(var hSuperClass)
		(= hSuperClass (self:superClass))

		(if (<> objectSpecies (send pObj:species) )
		    (if (IsObject(hSuperClass))
		    	return(send hSuperClass:isKindOf(pObj))
		    )
            return(FALSE)
		)
        return(TRUE)
	)
	(method (isMemberOf pObj)
        // Check if it's an instance
		(if (& (send pObj:{-info-}) $8000)
			(if ( not(& objectInfo $8000) )
		    	return(== objectSpecies (send pObj:species))
		    )
		)
	)
	(method (respondsTo aSelector)
		return(RespondsTo(self aSelector))
	)
	(method (yourself)
		return(self)
	)
)
/******************************************************************************/
(class Code of Obj
	(method (doit))
)
/******************************************************************************/
(class Collect of Obj
	(properties
		elements 0
		size 0
	)
	(method (dispose)
        (if(elements)
            (self:eachElementDo(#dispose))
            DisposeList(elements)
        )
        = elements 0
        = size 0
        (super:dispose())
	)
	(method (showStr strBuf)
        Format(strBuf "%s\n[Collection of size %d]" objectName size)
	)
	(method (showSelf)
		(var temps[40])
        Print( (self:showStr(@temps)) )
        (self:eachElementDo(#showSelf))
	)
	(method (add nodes)
		(var i)

        (if( not elements )
			 (= elements NewList())
        )
        (for (= i 0) (< i paramTotal) (++i)
            AddToEnd( elements NewNode(nodes[i] nodes[i]) )
		    ++size
        )
        return(self)
	)
	(method (delete nodes)
		(var i)
        (for (= i 0) ( < i paramTotal ) (++i)
            (if ( DeleteKey(elements nodes[i]) )
                --size
            )
        )
		return(self)
	)
	(method (eachElementDo aSelector sendParams)
		(var hNode, nextNode, nodeVal)

        = hNode FirstNode(elements)

        (while (hNode)
                = nextNode NextNode(hNode)
                = nodeVal NodeValue(hNode)
                (if( not IsObject(nodeVal) )
                     return
                )
                (send nodeVal:
                      aSelector(rest sendParams)
                )
                = hNode nextNode
          )
	)
	(method (firstTrue aSelector sendParams)
		(var hNode, nextNode, nodeVal)
        = hNode FirstNode(elements)

        (while (hNode)
            = nextNode NextNode(hNode)
            = nodeVal NodeValue(hNode)

            (if( (send nodeVal:aSelector(rest sendParams)) )
            	return(nodeVal)
            )
            = hNode nextNode
        )
        return(NULL)
	)
	(method (allTrue aSelector sendParams)
		(var hNode, nextNode, nodeVal)
        = hNode FirstNode(elements)
        (while (hNode)
            = nextNode NextNode(hNode)
            = nodeVal NodeValue(hNode)
            (if ( not (send nodeVal: aSelector(rest sendParams) ) )
                return(FALSE)
            )
            = hNode nextNode
      )
      return(TRUE)
	)
	(method (contains aNode)
         return( FindKey(elements aNode) )
	)
	(method (isEmpty)
         (if( (== elements 0) )
         	return(TRUE)
         )
         return(EmptyList(elements))
	)
	(method (first)
        return(FirstNode(elements))
	)
	(method (next pNode)
        return(NextNode(pNode))
	)
	(method (release)
		(var hNode, nextNode)
        = hNode FirstNode(elements)
        (while(hNode)
            = nextNode NextNode(hNode)
            (self:delete(NodeValue(hNode)))
            = hNode nextNode
         )
	)
)
/******************************************************************************/
(class List of Collect
	(properties
		elements 0
		size 0
	)
	(method (showStr strBuf)
         Format(strBuf "%s\n[List of size %d]" objectName size)
	)
	(method (at position)
		(var hNode)
		= hNode FirstNode(elements)
  		(while(position and hNode)
		    --position 
		    = hNode NextNode(hNode)
		)
		NodeValue(hNode)
	)
	(method (last)
         return(LastNode(elements))
	)
	(method (prev pNode)
         return(PrevNode(pNode))
	)
	(method (addToFront nodes)
		(var lastParam)

	    (if ( not elements )
             = elements NewList()
        )
        = lastParam (- paramTotal 1)
        (while ( <= 0 lastParam)
            AddToFront(elements NewNode(nodes[lastParam] nodes[lastParam]))

		    ++size
		    --lastParam
        )
	    return(self)
	)
	(method (addToEnd nodes)
		(var paramCounter)
        (if( not elements )
             = elements NewList()
        )
        = paramCounter 0
        (while (< paramCounter paramTotal)
            AddToEnd(elements NewNode(nodes[paramCounter] nodes[paramCounter]))

		    ++size
		    ++paramCounter
        )
        return(self)
	)
	(method (addAfter pNode nodes)
		(var i, hNode)
        = hNode FindKey(elements pNode)
        (if(hNode)
		    --paramTotal
            (for (= i 0) (< i paramTotal) (++i)
                = hNode AddAfter(elements hNode NewNode(nodes[i] nodes[i]))
                ++size
            )
        )
        return(self)
	)
	(method (indexOf pNode)
		(var i, hNode)
        = i 0
        = hNode FirstNode(elements)
        (while(hNode)
            (if(== pNode NodeValue(hNode))
                   return(i)
            )
            ++i
            = hNode NextNode(hNode)
		)
		return(-1)
	)
)
/******************************************************************************/
(class Set of List
	(properties
		elements 0
		size 0
	)
	(method (showStr strBuf)
         Format(strBuf "%s\n[Set of size %d]" objectName size)
	)
	(method (add nodes)
		(var i, hNode)
		(if(not elements)
			= elements NewList()
		)
  		(for (= i 0) (< i paramTotal) (++i)
			= hNode nodes[i]
			(if(not (self:contains(hNode)))
				AddToEnd(elements NewNode(hNode hNode))
				++size
			)
		)
	)
)
/******************************************************************************/
(class EventHandler of Set
	(properties
		elements 0
		size 0
	)
	(method (handleEvent pEvent)
		(var hNode, nextNode, nodeValue)
		= hNode FirstNode(elements)
		(while(hNode and not(send pEvent:claimed))
		    = nextNode NextNode(hNode)
		    (if( not IsObject( (= nodeValue NodeValue(hNode)) ) )
		    	break
		    )
		    (send nodeValue:handleEvent(pEvent))
			= hNode nextNode
		)
  		(send pEvent:claimed)
	)
)
/******************************************************************************/
(class Script of Obj
	(properties
		client 0
		state -1
		start 0
		timer 0
		cycles 0
		seconds 0
		lastSeconds 0
		register 0
		script 0
		caller 0
		next 0
	)
	(method (init theClient theCaller theRegister)
        (if(>= paramTotal 1)
		    = client theClient
            (send client:script(self))
            (if(>= paramTotal 2)
                = caller theCaller
                (if(>= paramTotal 3)
                    = register theRegister
                )
            )
        )
        (self:changeState(start))
	)
	(method (doit)
		(var theTime)
        (if(script)
            (send script:doit())
        )
        (if(cycles)
            (if( not --cycles )
                (self:cue())
            )
        )(else
		    (if(seconds)
                = theTime GetTime(gtTIME_OF_DAY)
                (if(<> theTime lastSeconds)
                    = lastSeconds theTime
                    (if( not --seconds )
                         (self:cue())
                    )
                )
            )
        )
	)
	(method (dispose)
		(var temp0)
		(if(IsObject(script))
		   	(send script:dispose())
		)
  		(if(IsObject(timer))
		    (send timer:dispose())
		)
		(if(IsObject(client))
			(if(IsObject(next))
				= temp0 next
			)(else
				(if(next)
					= temp0 ScriptID(next)
				)(else
					= temp0 0
				)
			)
			(send client:script(temp0))
			(if(not temp0)
				/*do nothing*/
		   	)(else
  				(if(== gRoomNumber gRoomNumberExit)
		   			(send temp0:init(client))
		   		)(else
  					(send temp0:dispose())
  				)
  			)
		)
		(if(IsObject(caller) and (== gRoomNumber gRoomNumberExit))
		    (send caller:cue(register))
  		)
  		= caller	NULL
  		= next		NULL
  		= client	NULL
  		= timer		NULL
  		= script	NULL
  		(super:dispose())
	)
	(method (changeState newState)
        = state newState
	)
	(method (cue sendParams)
        (self:
              changeState((+ state 1)  rest sendParams)
        )
	)
	(method (handleEvent pEvent)
        (if(script)
            (send script:handleEvent(pEvent))
        )
        return (send pEvent:claimed)
	)
	(method (setScript scriptObj sendParams)
         (if(IsObject(script))
             (send script:dispose())
         )
         (if(scriptObj)
             (send scriptObj:
                   init(self rest sendParams)
             )
		)
	)
)
/******************************************************************************/
(class Event of Obj
	(properties
		type 0
		message 0
		modifiers 0
		y 0
		x 0
		claimed 0
	)
	(method (new eventType)
		(var hEvent)
        = hEvent (super:new())

        (if(paramTotal)
		    GetEvent(eventType hEvent)
        )(else
		    GetEvent($7FFF hEvent)
        )
        return(hEvent)
	)
)
/******************************************************************************/
(procedure public (IsPosOrNeg aNumber)
	(if(< aNumber 0)
		return(-1)
	)
	return(> aNumber 0)
)
/******************************************************************************/
(procedure public (proc999_2 param1 param2)
		(var temp0)
		(if(<> paramTotal 1)
		    (if(< param1 (= temp0 proc999_2(rest param2)))
		    	return(temp0)
		    )
		)
		return(param1)
)
/******************************************************************************/
(procedure public (proc999_3 param1 params)
	(var temp0)
	(if( (== paramTotal 1) or (< param1 (= temp0 proc999_2(rest params))) )
		return(param1)
	)(else
  		return(temp0)
  	)
)
/******************************************************************************/
(procedure public (EqualsAny aNumber numbers)
	(var i)
	(for(= i 0) (< i (- paramTotal 1)) (++i)
		(if(== aNumber numbers[i])
			(if(aNumber)
				return(aNumber)
			)
			return(TRUE)
		)
	)
	return(FALSE)
)
/******************************************************************************/
