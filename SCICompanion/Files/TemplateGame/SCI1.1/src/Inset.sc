(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Actor")
(use "Obj")
(script INSET_SCRIPT)

/*
	An Inset lets you create a small window on the screen which can contain its own
	cast, and respond to its own events and such.
*/
(class Inset of Code
    (properties
        picture 0
        anOverlay 0
        style $0064
        view 0
        loop 0
        cel 0
        x 0
        y 0
        priority 14
        register 0
        hideTheCast 0
        caller 0
        owner 0
        script 0
        oldCast 0
        oldFeatures 0
        oldATPs 0
        oldMH 0
        oldKH 0
        oldDH 0
        oldWH 0
        oldObstacles 0
        oldStyle 0
        inset 0
        disposeNotOnMe 0
        modNum -1
        noun 0
        insetView 0
    )

	/*
		Initializes the Inset. Generally you would override this to initialize your Inset
		with the necessary Props and such. Then call (super:init(rest params)).
		
		:param heapPtr theCaller: Object that gets cue()'d when the Inset is disposed.
		:param heapPtr theOwner: Generally a room.
	*/
    (method (init theCaller theOwner theRegister)
        = owner theOwner
        (send owner:inset(self))
        = register theRegister
        = caller theCaller
        (if (hideTheCast)
            (self:hideCast(1))
        )
        = oldCast gOldCast
        = oldFeatures gOldFeatures
        = oldATPs gOldATPs
        = oldMH gOldMH
        = oldKH gOldKH
        = oldDH gOldDH
        = oldWH gOldWH
        = oldObstacles (send gRoom:obstacles)
        (send gRoom:obstacles((send ((List:new())):
                add()
                yourself()
            )
))
        = gOldCast (EventHandler:new())
        (send gOldCast:
            name("newCast")
            add()
        )
        = gOldFeatures (EventHandler:new())
        (send gOldFeatures:
            name("newFeatures")
            add(self)
        )
        = gOldATPs (EventHandler:new())
        (send gOldATPs:
            name("newATPs")
            add()
        )
        = gOldMH (EventHandler:new())
        (send gOldMH:
            name("newMH")
            add(self)
        )
        = gOldKH (EventHandler:new())
        (send gOldKH:
            name("newKH")
            add(self)
        )
        = gOldDH (EventHandler:new())
        (send gOldDH:
            name("newDH")
            add(self)
        )
        = gOldWH (EventHandler:new())
        (send gOldWH:
            name("newWH")
            add()
        )
        (send gTheDoits:add(self))
        (self:drawInset())
    )


    (method (doit)
        (if (script)
            (send script:doit())
        )
        (if (not hideTheCast)
            Animate((send oldCast:elements) 0)
        )
    )


    (method (dispose param1)
        (var theCaller)
        (send gOldFeatures:delete(self))
        (send gOldMH:delete(self))
        (send gOldKH:delete(self))
        (send gOldDH:delete(self))
        (send gOldWH:delete(self))
        (send gTheDoits:delete(self))
        (if (inset)
            (send inset:dispose(0))
        )
        (send gOldCast:
            eachElementDo(#dispose)
            eachElementDo(#delete)
            release()
            dispose()
        )
        (send gOldATPs:dispose())
        (send gOldFeatures:dispose())
        (send gOldMH:dispose())
        (send gOldKH:dispose())
        (send gOldDH:dispose())
        (send gOldWH:dispose())
        (send ((send gRoom:obstacles)):dispose())
        (send owner:inset(0))
        (if (not paramTotal or param1)
            (self:refresh())
        )
        (if (not paramTotal or param1)
            = gOldATPs oldATPs
            (send gOldATPs:doit())
        )
        (send gRoom:obstacles(oldObstacles))
        = gOldCast oldCast
        = gOldFeatures oldFeatures
        = gOldMH oldMH
        = gOldKH oldKH
        = gOldDH oldDH
        = gOldWH oldWH
        (if (hideTheCast)
            (self:hideCast(0))
        )
        (if ((not paramTotal or param1) and caller)
            = theCaller caller
            = caller 0
            (send theCaller:cue())
        )
    )


    (method (setScript theScript sendParams)
        (if (IsObject(script))
            (send script:dispose())
        )
        = script 
            (if (paramTotal)
                theScript
            )(else
                0
            )
        (if (script)
            (send script:init(self rest sendParams))
        )
    )


    (method (handleEvent pEvent)
        (var temp0[2])
        return 
            (if (inset and (send inset:handleEvent(pEvent)))
                0
            )(else
                (if (& (send pEvent:type) evVERB)
                    (if ((self:onMe(pEvent)))
                        (send pEvent:claimed(TRUE))
                        (self:doVerb((send pEvent:message)))
                    )(else
                        (if (disposeNotOnMe)
                            (send pEvent:claimed(TRUE))
                            (self:dispose())
                        )(else
                            return 0
                        )
                    )
                )
            )
    )


    (method (doVerb theVerb)
        (if (== modNum -1)
            = modNum gModNum
        )
        (if (global90 and Message(msgGET modNum noun theVerb 0 1))
            (send gTestMessager:say(noun theVerb 0 0 0 modNum))
        )
    )


    (method (hideCast param1)
        (var temp0, temp1)
        = temp0 0
        = temp1 
            (if (param1)
                1000
            )(else
                -1000
            )
        (while (< temp0 (send gOldCast:size))
            (send ((send gOldCast:at(temp0))):z((+ (send ((send gOldCast:at(temp0))):z) temp1)))
            ++temp0
        )
        Animate((send gOldCast:elements) 0)
    )


    (method (drawInset)
        (if (> picture 0)
            DrawPic(picture 
                (if (anOverlay)
                    100
                )(else
                    style
                )
 
                (if (anOverlay)
                    0
                )(else
                    1
                )
)
        )
        (if (view)
            = insetView (send ((inView:new())):
                    view(view)
                    loop(loop)
                    cel(cel)
                    x(x)
                    y(y)
                    setPri(priority)
                    ignoreActors(1)
                    init()
                    yourself()
                )
        )
    )


    (method (restore)
        (self:drawInset())
        (if (inset)
            (send ((send inset:oldATPs)):doit())
            (send inset:restore())
        )
    )


    (method (refresh)
        (if (view)
            DrawPic((send gRoom:picture) 100)
        )(else
            DrawPic((send gRoom:picture) style)
        )
        (send gRoom:style(oldStyle))
        (if (<> gPicNumber -1)
            DrawPic(gPicNumber 100 dpNO_CLEAR)
        )
        (if ((send gRoom:inset()))
            (send ((send gRoom:inset())):restore())
        )
    )

	/*
	.. function:: setInset([theInset theCaller theRegister])
	
		Sets an :class:`Inset` on this Inset! To clear the inset, pass no parameters.
		
		:param heapPtr theInset: The Inset instance.
		:param heapPtr theCaller: An object that will get cue()'d when the Inset is disposed.
		
		Example usage::
		
			(send myInset:setInset(anotherSubInset))
	*/
    (method (setInset theInset theCaller theRegister)
        (if (inset)
            (send inset:dispose())
        )
        (if (paramTotal and theInset)
            (send theInset:init(
                (if (>= paramTotal 2)
                    theCaller
                )(else
                    0
                )
 				self 
                (if (>= paramTotal 3)
                    theRegister
                )(else
                    0
                )
							 )
			)
        )
    )

	/*
	.. function:: onMe(theObj)
	
	.. function:: onMe(x y)
	
	If the Inset has a view, return true if the object is on that view.
	
		:param heapPtr theObj: An object with x and y properties.
		:param number x: The x coordinate.
		:param number y: The y coordinate.
		:returns: TRUE if the object is on the Inset's view. If there is no view, returns TRUE.
	*/
    (method (onMe param1 param2)
        (var temp0, temp1)
        (if (IsObject(param1))
            = temp0 (send param1:x)
            = temp1 (send param1:y)
        )(else
            = temp0 param1
            = temp1 param2
        )
        return 
            (if (view)
                return (send insetView:onMe(param1 param2))
            )(else
                return TRUE
            )
    )
)

(instance inView of View
    (properties)

    (method (handleEvent)
        return 0
    )

)
