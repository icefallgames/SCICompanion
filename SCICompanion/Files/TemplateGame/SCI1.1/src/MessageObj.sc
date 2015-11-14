/*
	This script contains :class:`Conversation`, which lets you assemble multiple messages together
	in a sequence, even if they have separate noun/verb/cond/seq tuples.
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Print")
(use "Obj")
(script MESSAGEOBJ_SCRIPT)


/*
	This class works in
	conjunction with :class:`Conversation`.
*/
(class MessageObj of Obj
    (properties
        modNum -1
        noun 0
        verb 0
        cond 0
        sequence 0
        whoSays 0
        client 0
        caller 0
        font 0
        x 0
        y 0
    )

    (method (showSelf)
        (var temp0[40])
        = whoSays (send gTestMessager:findTalker(Message(msgGET modNum noun verb cond 
                    (if (sequence)
                    )(else
                        1
                    )
)))
        (if (not IsObject(whoSays))
            (Print:
                addTextF("<MessageObj> Message not found: %d - %d, %d, %d, %d" modNum noun verb cond sequence)
                init()
            )
            = gQuitGame TRUE
        )(else
            (if (font)
                (send whoSays:font(font))
            )
            (if (x or y)
                (send whoSays:
                    x(x)
                    y(y)
                )
            )
            (send gTestMessager:say(noun verb cond sequence caller modNum))
        )
    )

)

/*
	This seems to be a class to which you can add a series of messages that will be displayed one
	after another. :class:`Messager` does this automatically, but only if messages have the same noun/verb/cond tuple
	and sequentially increasing sequence numbers.
*/
(class Conversation of List
    (properties
        elements 0
        size 0
        script 0
        curItem -1
        caller 0
    )

    (method (init theCaller)
        = curItem -1
        (if (paramTotal and IsObject(theCaller))
            = caller theCaller
        )
        (send gTheDoits:add(self))
        (self:cue())
    )


    (method (doit)
        (if (script)
            (send script:doit())
        )
    )


    (method (dispose)
        (var theCaller)
        (self:eachElementDo(#perform cleanCode))
        (send gTheDoits:delete(self))
        (if (gDialog)
            (send gDialog:dispose())
        )
        (if (script)
            = script 0
        )
        = theCaller caller
        (super:dispose())
        (if (theCaller)
            (send theCaller:cue())
        )
    )

	/*
	.. function:: add([moduleNumber noun verb condition sequence x y font)
	
		Adds a new message to the conversation.
		
		:param number moduleNumber: Room number, or -1 for the current room.
		:param number noun: The message noun.
		:param number verb: The message verb.
		:param number condition: The message condition.
		:param number sequence: The message sequence.
		:param number x: The message x position.
		:param number y: The message y position.
		:param number font: The message font.
	*/
    (method (add param param2)
        (var theGModNum, theNoun, theVerb, theCase, theSeq, theX, theY, theFont)
        = theSeq 0
        = theCase theSeq
        = theVerb theCase
        = theNoun theVerb
        = theGModNum theNoun
        = theFont 0
        = theY theFont
        = theX theY
        (if (paramTotal and not IsObject(param[0]))
            = theGModNum param[0]
            (if (== theGModNum -1)
                = theGModNum gModNum
            )
            (if (> paramTotal 1)
                = theNoun param[1]
                (if (> paramTotal 2)
                    = theVerb param[2]
                    (if (> paramTotal 3)
                        = theCase param[3]
                        (if (> paramTotal 4)
                            = theSeq param[4]
                            (if (> paramTotal 5)
                                = theX param[5]
                                (if (> paramTotal 6)
                                    = theY param[6]
                                    (if (> paramTotal 7)
                                        = theFont param[7]
                                    )
                                )
                            )
                        )
                    )
                )
            )
            (if (not IsObject(param[0]))
                (super:add((send ((MessageObj:new())):
                        modNum(theGModNum)
                        noun(theNoun)
                        verb(theVerb)
                        cond(theCase)
                        sequence(theSeq)
                        x(theX)
                        y(theY)
                        font(theFont)
                        yourself()
                    )
						  )
				)
            )
        )(else
            (super:add(param rest param2))
        )
    )


    (method (cue param1)
        (var temp0, temp1)
        (if ((paramTotal and param1) or (== ++curItem size))
            (self:dispose())
        )(else
            = temp0 (self:at(curItem))
            (if ((send temp0:isKindOf(MessageObj)))
                (send temp0:
                    caller(self)
                    showSelf()
                )
            )(else
                (if ((send temp0:isKindOf(Script)))
                    (self:setScript(temp0 self))
                )(else
                    (if (IsObject(temp0))
                        (send temp0:doit(self))
                    )(else
                        (self:cue())
                    )
                )
            )
        )
    )


    (method (setScript theScript sendParams)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (theScript)
            (send theScript:init(self rest sendParams))
        )
    )


    (method (load param1)
        (var theGModNum, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8)
        = theGModNum GetValueAt(param1 0)
        = temp1 GetValueAt(param1 1)
        = temp2 GetValueAt(param1 2)
        = temp3 GetValueAt(param1 3)
        = temp4 GetValueAt(param1 4)
        = temp5 GetValueAt(param1 5)
        = temp6 GetValueAt(param1 6)
        = temp7 GetValueAt(param1 7)
        = temp8 7
        (while (theGModNum)
            (if (== theGModNum -1)
                = theGModNum gModNum
            )
            (self:add(theGModNum temp1 temp2 temp3 temp4 temp5 temp6 temp7))
            = theGModNum GetValueAt(param1 ++temp8)
            = temp1 GetValueAt(param1 ++temp8)
            = temp2 GetValueAt(param1 ++temp8)
            = temp3 GetValueAt(param1 ++temp8)
            = temp4 GetValueAt(param1 ++temp8)
            = temp5 GetValueAt(param1 ++temp8)
            = temp6 GetValueAt(param1 ++temp8)
            = temp7 GetValueAt(param1 ++temp8)
        )
    )
)

(instance cleanCode of Code
    (properties)

    (method (doit param1)
        (var temp0)
        (if ((send param1:isKindOf(Script)))
            (send param1:caller(0))
        )
        (if (((send param1:isKindOf(MessageObj)) and IsObject((= temp0 (send param1:whoSays)))) and (send temp0:underBits))
            (send temp0:dispose(1))
        )
    )
)
