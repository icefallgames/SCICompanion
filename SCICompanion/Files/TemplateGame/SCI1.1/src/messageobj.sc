(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Obj")
(script 925)



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

// This seems to be a class to which you can add a series of messages that will be displayed one
// after another. gTestMessager can do the same thing.
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


    (method (add theTheGModNum_2 param2)
        (var theGModNum, theTheGModNum, theTheTheGModNum, theTheTheGModNum_2, theTheTheGModNum_2_2, theTheTheGModNum_2_3, theTheTheGModNum_2_4, theTheTheGModNum_2_5)
        = theTheTheGModNum_2_2 0
        = theTheTheGModNum_2 theTheTheGModNum_2_2
        = theTheTheGModNum theTheTheGModNum_2
        = theTheGModNum theTheTheGModNum
        = theGModNum theTheGModNum
        = theTheTheGModNum_2_5 0
        = theTheTheGModNum_2_4 theTheTheGModNum_2_5
        = theTheTheGModNum_2_3 theTheTheGModNum_2_4
        (if (paramTotal and not IsObject(theTheGModNum_2[0]))
            = theGModNum theTheGModNum_2[0]
            (if (== theGModNum -1)
                = theGModNum gModNum
            )
            (if (> paramTotal 1)
                = theTheGModNum theTheGModNum_2[1]
                (if (> paramTotal 2)
                    = theTheTheGModNum theTheGModNum_2[2]
                    (if (> paramTotal 3)
                        = theTheTheGModNum_2 theTheGModNum_2[3]
                        (if (> paramTotal 4)
                            = theTheTheGModNum_2_2 theTheGModNum_2[4]
                            (if (> paramTotal 5)
                                = theTheTheGModNum_2_3 theTheGModNum_2[5]
                                (if (> paramTotal 6)
                                    = theTheTheGModNum_2_4 theTheGModNum_2[6]
                                    (if (> paramTotal 7)
                                        = theTheTheGModNum_2_5 theTheGModNum_2[7]
                                    )
                                )
                            )
                        )
                    )
                )
            )
            (if (not IsObject(theTheGModNum_2[0]))
                (super:add((send ((MessageObj:new())):
                        modNum(theGModNum)
                        noun(theTheGModNum)
                        verb(theTheTheGModNum)
                        case(theTheTheGModNum_2)
                        sequence(theTheTheGModNum_2_2)
                        x(theTheTheGModNum_2_3)
                        y(theTheTheGModNum_2_4)
                        font(theTheTheGModNum_2_5)
                        yourself()
                    )
))
            )
        )(else
            (super:add(theTheGModNum_2 rest param2))
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
        = theGModNum proc999_6(param1 0)
        = temp1 proc999_6(param1 1)
        = temp2 proc999_6(param1 2)
        = temp3 proc999_6(param1 3)
        = temp4 proc999_6(param1 4)
        = temp5 proc999_6(param1 5)
        = temp6 proc999_6(param1 6)
        = temp7 proc999_6(param1 7)
        = temp8 7
        (while (theGModNum)
            (if (== theGModNum -1)
                = theGModNum gModNum
            )
            (self:add(theGModNum temp1 temp2 temp3 temp4 temp5 temp6 temp7))
            = theGModNum proc999_6(param1 ++temp8)
            = temp1 proc999_6(param1 ++temp8)
            = temp2 proc999_6(param1 ++temp8)
            = temp3 proc999_6(param1 ++temp8)
            = temp4 proc999_6(param1 ++temp8)
            = temp5 proc999_6(param1 ++temp8)
            = temp6 proc999_6(param1 ++temp8)
            = temp7 proc999_6(param1 ++temp8)
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
