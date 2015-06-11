(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "PolyPath")
(use "Feature")
(use "Cycle")
(use "Obj")
(script 998)



(class View of Feature
    (properties
        x 0
        y 0
        z 0
        heading 0
        noun 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0101
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
    )

    (method (init)
        (var temp0)
        = temp0 
            (if (& signal $0020)
                gOldATPs
            )(else
                gOldCast
            )
        = signal (& signal $7fff)
        (if (not (send temp0:contains(self)))
            = lsRight (= lsBottom (= lsLeft (= lsTop 0)))
            = signal (& signal $ff77)
        )
        BaseSetter(self)
        (send temp0:add(self))
        (if (== temp0 gOldATPs)
            (if (not & signal fixPriOn)
                = priority CoordPri(y)
            )
            SetNowSeen(self)
            (send temp0:doit())
        )
        (self:
            initialize()
            checkDetail()
        )
    )


    (method (dispose)
        (self:
            startUpd()
            hide()
        )
        = signal (| signal $8000)
    )


    (method (showSelf)
        (Print:
            addText(name)
            addIcon(view loop cel)
            init()
        )
    )


    (method (isNotHidden)
        return not & signal $0088
    )


    (method (onMe param1 param2)
        (var temp0, temp1)
        (if (IsObject(param1))
            = temp0 (send param1:x)
            = temp1 (send param1:y)
        )(else
            = temp0 param1
            = temp1 param2
        )
        (if (& signal $0080)
            0
        )(else
            (if (not IsObject(onMeCheck) and (& signal noCycler))
                (if (not 
                (if ((nsLeft or nsRight) or nsTop)
                )(else
                    nsBottom
                )
 or ((((<= nsLeft temp0) and (<= temp0 nsRight)) and (<= nsTop temp1)) and (<= temp1 nsBottom)))
                    not IsItSkip(view loop cel - temp1 nsTop - temp0 nsLeft)
                )
            )(else
                (super:onMe(temp0 temp1))
            )
        )
    )


    (method (posn theX theY theZ)
        (if (>= paramTotal 1)
            = x theX
            (if (>= paramTotal 2)
                = y theY
                (if (>= paramTotal 3)
                    = z theZ
                )
            )
        )
        BaseSetter(self)
        (self:forceUpd())
    )


    (method (stopUpd)
        = signal (| signal notUpd)
        = signal (& signal $fffd)
    )


    (method (forceUpd)
        = signal (| signal $0040)
    )


    (method (startUpd)
        = signal (| signal $0002)
        = signal (& signal $fffe)
    )


    (method (setPri thePriority)
        (if (== paramTotal 0)
            = signal (| signal fixPriOn)
        )(else
            (if (== thePriority -1)
                = signal (& signal $ffef)
            )(else
                = priority thePriority
                = signal (| signal fixPriOn)
            )
        )
        (self:forceUpd())
    )


    (method (setLoop theLoop)
        (if (== paramTotal 0)
            = signal (| signal noTurn)
        )(else
            (if (== theLoop -1)
                = signal (& signal $f7ff)
            )(else
                = loop theLoop
                = signal (| signal noTurn)
            )
        )
        (self:forceUpd())
    )


    (method (setCel param1)
        (if (== paramTotal 0)
            0
        )(else
            (if (== param1 -1)
                0
            )(else
                = cel 
                    (if (>= param1 (self:lastCel()))
                        (self:lastCel())
                    )(else
                        param1
                    )
            )
        )
        (self:forceUpd())
    )


    (method (ignoreActors param1)
        (if ((== 0 paramTotal) or param1)
            = signal (| signal ignAct)
        )(else
            = signal (& signal $bfff)
        )
    )


    (method (hide)
        = signal (| signal $0008)
    )


    (method (show)
        = signal (& signal $fff7)
    )


    (method (delete)
        (if (& signal $8000)
            = signal (& signal $7fff)
            (if ((send gOldATPs:contains(self)))
                (send gOldATPs:delete(self))
                = signal (& signal $ffdf)
            )(else
                (if (& signal $0020)
                    (send gOldCast:delete(self))
                    (send gOldATPs:add(self))
                    return 
                )(else
                    (send gOldCast:delete(self))
                )
            )
            (if (underBits)
                UnLoad(133 underBits)
                = underBits 0
            )
            (super:dispose())
            (if (IsObject(actions))
                (send actions:dispose())
            )
            = actions 0
        )
    )


    (method (addToPic)
        (if ((send gOldCast:contains(self)))
            = signal (| signal $8021)
        )(else
            = signal (| signal $0020)
            (self:init())
        )
    )


    (method (lastCel)
        return - NumCels(self) 1
    )


    (method (motionCue)
    )


    (method (checkDetail)
    )


    (method (setScale param1)
        (var temp0, temp1, temp2, temp3[40])
        (if (not paramTotal)
            = scaleSignal (| scaleSignal $0001)
            = scaleSignal (& scaleSignal $fffd)
        )(else
            (if (not param1)
                = scaleSignal (& scaleSignal $fffc)
            )(else
                (if (< param1 (send gRoom:vanishingY))
                    FormatPrint("<%s setScale:> y value less than vanishingY" name)
                )(else
                    = temp0 (- param1 (send gRoom:vanishingY))
                    = temp1 (- 190 param1)
                    = temp2 (+ (/ (* temp1 100) temp0) 100)
                    = scaleSignal (| scaleSignal $0003)
                    = maxScale (/ (* temp2 128) 100)
                )
            )
        )
    )

)
(class Prop of View
    (properties
        x 0
        y 0
        z 0
        heading 0
        noun 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0000
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
        cycleSpeed 6
        script 0
        cycler 0
        timer 0
        detailLevel 0
        scaler 0
    )

    (method (doit)
        (var temp0)
        (if (& signal $8000)
            return 
        )
        (if (script)
            (send script:doit())
        )
        (if ((& signal $0004) and not (& signal $0002))
            return 
        )
        (if (cycler)
            (send cycler:doit())
        )
        (if (scaler)
            (send scaler:doit())
        )
    )


    (method (handleEvent pEvent)
        (if (script)
            (send script:handleEvent(pEvent))
        )
        (super:handleEvent(pEvent))
    )


    (method (delete)
        (if (& signal $8000)
            (self:
                setScript(0)
                setCycle(0)
            )
            (if (timer)
                (send timer:dispose())
            )
            (if (IsObject(scaler))
                (send scaler:dispose())
                = scaler 0
            )
            (super:delete())
        )
    )


    (method (motionCue)
        (if (cycler and (send cycler:completed))
            (send cycler:motionCue())
        )
    )


    (method (checkDetail param1)
        (if (not detailLevel)
        )(else
            (if (< 
            (if (paramTotal)
                param1
            )(else
                (send gGame:detailLevel())
            )
 detailLevel)
                (self:stopUpd())
            )(else
                (if (cycler)
                    (self:startUpd())
                )
            )
        )
    )


    (method (setScale param1 param2 param3)
        (if (scaler)
            (send scaler:dispose())
            = scaler 0
        )
        (if (not paramTotal)
            (super:setScale())
        )(else
            (if (IsObject(param1))
                = scaleSignal (| scaleSignal $0001)
                = scaleSignal (& scaleSignal $fffd)
                = scaler 
                    (if (& (send param1:{-info-}) $8000)
                        (send param1:new())
                    )(else
                        param1
                    )
                (send scaler:init(self param2 rest param3))
            )(else
                (if (== param1 -1)
                    (if ((send param2:scaleSignal))
                        = scaleSignal (send param2:scaleSignal)
                        = maxScale (send param2:maxScale)
                        (if (IsObject((send param2:scaler)))
                            (send (= scaler (send ((send param2:scaler)):new())):client(self))
                        )
                    )
                )(else
                    (super:setScale(param1))
                )
            )
        )
    )


    (method (setCycle theCycler sendParams)
        (if (cycler)
            (send cycler:dispose())
        )
        (if (theCycler)
            (self:startUpd())
            = cycler 
                (if (& (send theCycler:{-info-}) $8000)
                    (send theCycler:new())
                )(else
                    theCycler
                )
            (send cycler:init(self rest sendParams))
        )(else
            = cycler 0
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


    (method (cue)
        (if (script)
            (send script:cue())
        )
    )

)
(class Actor of Prop
    (properties
        x 0
        y 0
        z 0
        heading 0
        noun 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0000
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
        cycleSpeed 6
        script 0
        cycler 0
        timer 0
        detailLevel 0
        scaler 0
        illegalBits $8000
        xLast 0
        yLast 0
        xStep 3
        origStep 770
        moveSpeed 6
        blocks 0
        baseSetter 0
        mover 0
        looper 0
        viewer 0
        avoider 0
        code 0
    )

    (method (init param1)
        (super:init(rest param1))
        = xLast x
        = yLast y
    )


    (method (doit)
        (var temp0, theBrLeft, theBrRight, temp3, temp4, temp5, temp6, temp7)
        (if (& signal $8000)
            return 
        )
        (if (script)
            (send script:doit())
        )
        (if (code)
            (send code:doit(self))
        )
        (if ((& signal $0004) and not (& signal $0002))
            return 
        )
        (if (viewer)
            (send viewer:doit(self))
        )
        (if (avoider)
            (send avoider:doit())
        )
        (if (mover)
            (if ((& scaleSignal $0001) and not (& scaleSignal $0004))
                = temp5 (>> origStep $0008)
                = temp6 (& origStep $00ff)
                = temp7 (/ (* temp5 scaleX) 128)
                = temp3 
                    (if (temp7)
                    )(else
                        1
                    )
                = temp7 (/ (* temp6 scaleY) 128)
                = temp4 
                    (if (temp7)
                    )(else
                        1
                    )
                (if ((<> temp3 xStep) or (<> temp4 yStep))
                    (self:setStep(temp3 temp4 1))
                )
            )
            (if (mover)
                (send mover:doit())
            )
        )
        (if (scaler)
            (send scaler:doit())
        )
        (if (cycler)
            = theBrLeft brLeft
            = theBrRight brRight
            (send cycler:doit())
            (if (baseSetter)
                (send baseSetter:doit(self))
            )(else
                BaseSetter(self)
            )
        )
        = xLast x
        = yLast y
    )


    (method (posn theXLast theYLast param3)
        (super:posn(theXLast theYLast rest param3))
        = xLast theXLast
        = yLast theYLast
    )


    (method (setLoop param1 param2)
        (var theLooper)
        = theLooper 
            (if (== paramTotal 0)
                (super:setLoop())
                0
            )(else
                (if (not IsObject(param1))
                    (super:setLoop(param1 rest param2))
                    0
                )(else
                    (if (& (send param1:{-info-}) $8000)
                        (send param1:new())
                    )(else
                        param1
                    )
                )
            )
        (if (theLooper)
            (if (looper)
                (send looper:dispose())
            )
            (send (= looper theLooper):init(self rest param2))
        )
    )


    (method (delete)
        (if (& signal $8000)
            (if (<> mover -1)
                (self:setMotion(0))
            )
            (self:setAvoider(0))
            (if (baseSetter)
                (send baseSetter:dispose())
                = baseSetter 0
            )
            (if (looper)
                (send looper:dispose())
                = looper 0
            )
            (if (viewer)
                (send viewer:dispose())
                = viewer 0
            )
            (if (blocks)
                (send blocks:dispose())
                = blocks 0
            )
            (if (code)
                (send code:dispose())
                = code 0
            )
            (if (IsObject(actions))
                (send actions:dispose())
                = actions 0
            )
            (super:delete())
        )
    )


    (method (motionCue)
        (if (mover and (send mover:completed))
            (send mover:motionCue())
        )
        (super:motionCue())
    )


    (method (checkDetail param1)
        (if (not detailLevel)
        )(else
            (if (< 
            (if (paramTotal)
                param1
            )(else
                (send gGame:detailLevel())
            )
 detailLevel)
                (self:stopUpd())
            )(else
                (if (cycler or mover)
                    (self:startUpd())
                )
            )
        )
    )


    (method (setMotion theMover sendParams)
        (if (mover and (<> mover -1))
            (send mover:dispose())
        )
        (if (theMover)
            (self:startUpd())
            = mover 
                (if (& (send theMover:{-info-}) $8000)
                    (send theMover:new())
                )(else
                    theMover
                )
            (send mover:init(self rest sendParams))
        )(else
            = mover 0
        )
    )


    (method (setAvoider theAvoider sendParams)
        (if (avoider)
            (send avoider:dispose())
        )
        = avoider 
            (if (IsObject(theAvoider) and (& (send theAvoider:{-info-}) $8000))
                (send theAvoider:new())
            )(else
                theAvoider
            )
        (if (avoider)
            (send avoider:init(self rest sendParams))
        )
    )


    (method (ignoreHorizon param1)
        (if (not paramTotal or param1)
            = signal (| signal ignoreHorizon)
        )(else
            = signal (& signal $dfff)
        )
    )


    (method (observeControl bits)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            = illegalBits (| illegalBits bits[temp0])
            ++temp0
        )
    )


    (method (ignoreControl bits)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            = illegalBits (& illegalBits bnot bits[temp0])
            ++temp0
        )
    )


    (method (observeBlocks param1)
        (if (not blocks)
            = blocks (Set:new())
        )
        (send blocks:add(rest param1))
    )


    (method (ignoreBlocks param1)
        (if (blocks)
            (send blocks:delete(rest param1))
            (if ((send blocks:isEmpty()))
                (send blocks:dispose())
                = blocks 0
            )
        )
    )
    
    (method (isStopped)
    	(if (IsObject(mover))
    		(if ((== x (send mover:xLast)) and (== y send mover:yLast))
    			return TRUE
			)
			return FALSE
		)
		return TRUE
    )

    (method (isBlocked)
        return & signal $0400
    )


    (method (inRect param1 param2 param3 param4)
        return 
            (if (((<= param1 x) and (<= x param3)) and (<= param2 y))
                <= y param4
            )(else
                0
            )
    )


    (method (onControl fUsePoint)
        (if (paramTotal and fUsePoint)
            OnControl(4 x y)
        )(else
            OnControl(4 brLeft brTop brRight brBottom)
        )
    )


    (method (distanceTo pObj)
        GetDistance(x y (send pObj:x) (send pObj:y) gPicAngle)
    )


    (method (cantBeHere)
        (var temp0)
        (if (baseSetter)
            (send baseSetter:doit(self))
        )(else
            BaseSetter(self)
        )
        = temp0 
            (if (CanBeHere(self (send gOldCast:elements)))
            )(else
                (if ((not (& signal ignoreHorizon) and IsObject(gRoom)) and (< y (send gRoom:horizon)))
                    -1
                )(else
                    (if (blocks and not (send blocks:allTrue(57 self)))
                        -2
                    )
                )
            )
    )


    (method (setStep newX newY param3)
        (var theXStep, theYStep)
        = theXStep (>> origStep $0008)
        = theYStep (& origStep $00ff)
        (if ((>= paramTotal 1) and (<> newX -1))
            = theXStep newX
        )
        (if ((>= paramTotal 2) and (<> newY -1))
            = theYStep newY
        )
        (if ((< paramTotal 3) or not param3)
            = origStep (+ (<< theXStep $0008) theYStep)
        )
        = xStep theXStep
        = yStep theYStep
        (if (IsObject(mover) and ((send mover:isMemberOf(MoveTo)) or (send mover:isMemberOf(PolyPath))))
            (send mover:init())
        )
    )


    (method (setDirection newDirection)
        (var temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7)
        = temp1 (send gRoom:vanishingY)
        = temp0 
            (if (== temp1 -30000)
                x
            )(else
                (send gRoom:vanishingX)
            )
        (if ((== xStep 0) and (== yStep 0))
            return 
        )
        = temp5 (/ 32000 Max(xStep yStep))
        (switch (newDirection)
            (case 0
                (self:setMotion(0))
                return 
            )
            (case 1
                = temp2 (- temp0 x)
                = temp3 (- temp1 y)
            )
            (case 5
                = temp2 (- x temp0)
                = temp3 (- y temp1)
            )
            (case 3
                = temp2 temp5
                = temp3 0
            )
            (case 7
                = temp2 neg temp5
                = temp3 0
            )
            (default 
                = temp4 GetAngle(x y temp0 temp1)
                (if (< 180 temp4)
                    = temp4 (- temp4 360)
                )
                = temp4 (+ (/ (+ temp4 90) 2) (* 45 (- newDirection 2)))
                = temp2 SinMult(temp4 100)
                = temp3 neg CosMult(temp4 100)
            )
        )
        = temp5 (/ temp5 5)
        (while ((< Abs(temp3) temp5) and (< Abs(temp2) temp5))
            = temp2 (* temp2 5)
            = temp3 (* temp3 5)
        )
        = temp7 (send gRoom:obstacles)
        (if (temp7 and global67)
            = temp6 AvoidPath(x y (+ x temp2) (+ y temp3) (send temp7:elements) (send temp7:size) 0)
            = temp2 (- proc999_6(temp6 2) x)
            = temp3 (- proc999_6(temp6 3) y)
            Memory(memFREE temp6)
        )
        (if (temp2 or temp3)
            (self:setMotion(MoveTo (+ x temp2) (+ y temp3)))
        )(else
            (if (newDirection)
                (self:
                    setMotion(0)
                    setHeading(* (- newDirection 1) 45)
                )
            )(else
                (self:setMotion(0))
            )
        )
    )


    (method (setHeading theHeading param2 param3)
        (if (paramTotal)
            = heading theHeading
        )
        (if (looper)
            (send looper:doit(self heading 
                (if (>= paramTotal 2)
                    param2
                )(else
                    0
                )
))
        )(else
            DirLoop(self heading)
            (if ((>= paramTotal 2) and IsObject(param2))
                (send param2:cue(rest param3))
            )
        )
        return heading
    )


    (method (setSpeed newSpeed)
        (if (paramTotal)
            = moveSpeed (= cycleSpeed newSpeed)
        )
        return moveSpeed
    )

)
