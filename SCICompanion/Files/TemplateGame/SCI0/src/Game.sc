/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 game.sc
 Contains the general game functions, including the game handler class, 
 region/room classes and statusbar class.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script GAME_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "sound")
(use "syswindow")
(use "cycle")
(use "inv")
(use "user")
(use "obj")
/******************************************************************************/
(instance cast of EventHandler
	(properties)
)
/******************************************************************************/
(instance features of EventHandler
	(properties)
)
/******************************************************************************/
(instance sFeatures of EventHandler
	(properties)
	(method (delete param1)
		(super:delete(param1))
	)
)
/******************************************************************************/
(instance sounds of EventHandler
	(properties)
)
/******************************************************************************/
(instance regions of EventHandler
	(properties)
)
/******************************************************************************/
(instance locales of EventHandler
	(properties)
)
/******************************************************************************/
(instance addToPics of EventHandler
	(properties)
	(method (doit)
  		AddToPic(elements)
	)
)
/******************************************************************************/
(instance _controls of Controls
	(properties)
)
/******************************************************************************/
(instance timers of Set
	(properties)
)
/******************************************************************************/
(class Game of Obj
	(properties
		script 0
	)
	(method (init)
		(send (= gCast cast):add())
		(send (= gFeatures features):add())
		(send (= gSFeatures sFeatures):add())
		(send (= gSounds sounds):add())
		(send (= gRegions regions):add())		
		(send (= gLocales locales):add())
		(send (= gAddToPics addToPics):add())
		(send (= gTimers timers):add())
		
		= gSaveDirPtr GetSaveDir()
		
		(Inv:init())
		(User:init())
	)
	(method (doit)
		(send gSounds:eachElementDo(#check))
		(send gTimers:eachElementDo(#doit))
		(if(gPrintDlg)
			(send gPrintDlg:check())
		)
		Animate((send gCast:elements) 1)
		(if(gCastMotionCue)
			= gCastMotionCue FALSE
			(send gCast:eachElementDo(#motionCue))
		)
		(if(script)
			(send script:doit())
		)
		(send gRegions:eachElementDo(#doit))
		(if(== gRoomNumber gRoomNumberExit)
		    (User:doit())
		)
		(if(<> gRoomNumber gRoomNumberExit)
			(self:newRoom(gRoomNumber))
		)
		(send gTimers:eachElementDo(#delete))

		return(GameIsRestarting(FALSE))
	)
	(method (showSelf)
		(send gRegions:showSelf())
	)
	(method (play)
		= gGame self	
		= gSaveDirPtr GetSaveDir()
		(if( (not GameIsRestarting()) )
		    GetCWD(gSaveDirPtr)
		)
		(self:setCursor(gLoadingCursor 1))
		(self:init())
		(self:setCursor(gNormalCursor HaveMouse()))
		(while( (not gQuitGame) )
		    (self:doit())
			Wait(gSpeed)
		)
	)
	(method (replay)
		(if(gUserEvent)
			(send gUserEvent:dispose())
		)
		(send gSFeatures:release())
		(if(gPrintDlg)
			(send gPrintDlg:dispose())
		)
		(send gCast:eachElementDo(#perform RU))
		(send gGame:setCursor(gLoadingCursor 1))

		DrawPic((send gRoom:curPic) 100 dpCLEAR gDefaultPalette)

		(if(<> gOverlay -1)
		    DrawPic(gOverlay 100 dpNO_CLEAR gDefaultPalette)
		)
		(if( (send gRoom:controls) )
			(send (send gRoom:controls):draw())
		)
		(send gAddToPics:doit())
		(send gGame:setCursor(gNormalCursor HaveMouse()))
		
		(SL:doit())
		DoSound(sndRESUME)
		(Sound:pause(0))

		(while( (not gQuitGame) )
		    (self:doit())
			Wait(gSpeed)
		)
	)
	(method (newRoom newRoomNum)
		(var oldCursor, hEvent)
		(send gAddToPics:dispose())
		(send gFeatures:
			eachElementDo(#dispose)
			release()
		)
		(send gCast:
			eachElementDo(#dispose)
			eachElementDo(#delete)
		)
		(send gTimers:
			eachElementDo(#delete)
		)
		(send gRegions:
			eachElementDo(#perform DNKR)
			release()			
		)
		(send gLocales:
			eachElementDo(#dispose)
			release()
		)
		Animate(0)
		= gPreviousRoomNumber	gRoomNumberExit
		= gRoomNumberExit		newRoomNum
		= gRoomNumber			newRoomNum
		FlushResources(newRoomNum)
		= oldCursor (self:setCursor(gLoadingCursor 1))
		(self:
			startRoom(gRoomNumberExit)
			checkAni()
			setCursor(oldCursor HaveMouse())
		)
		SetSynonyms(gRegions)
  		(while(send (= hEvent (Event:new(evMOUSE))):type)
  			(send hEvent:dispose())
		)
		(send hEvent:dispose())
	)
	(method (startRoom roomNum)
		(if(gDebugOnExit)
			SetDebug()
		)
		(send gRegions:addToFront(= gRoom ScriptID(roomNum)))
		(send gRoom:init())
	)
	(method (restart)
		(if(gPrintDlg)
			(send gPrintDlg:dispose())
		)
		RestartGame()
	)
	(method (restore strDescription)
		(var gameNum = -1, oldCursor, hSound)
		Load(rsFONT gSaveRestoreFont)
		Load(rsCURSOR gNormalCursor)
		Load(rsCURSOR gLoadingCursor)

		= oldCursor (self:setCursor(gNormalCursor))
		= hSound (Sound:pause(1))

		(if(GetSaveDisk(TRUE))
			(if(gPrintDlg)
				(send gPrintDlg:dispose())
			)
			= gameNum (Restore:doit(rest strDescription))
			(if(<> gameNum -1)
				(self:setCursor(gLoadingCursor))
				(if(CheckSaveGame(objectName gameNum gVersion))
					(send gCast:eachElementDo(#dispose))
					(send gCast:eachElementDo(#delete))
					RestoreGame(objectName gameNum gVersion)
				)
  				(else
  					Print(
  						"That game was saved under a different interpreter. It cannot be restored."
  						#font 0
  						#button "OK" 1
  					)
  					(self:setCursor(oldCursor HaveMouse()))
  					= gameNum -1
				)
  			)
  			GetSaveDisk(FALSE)
		)
		(Sound:pause(hSound))
		return(gameNum)
	)
	(method (save)
		(var strDescBuf[20], gameNum, oldCursor, hSound)
		Load(rsFONT gSaveRestoreFont)
		Load(rsCURSOR gLoadingCursor)
		
		= oldCursor (self:setCursor(gNormalCursor))
		= hSound (Sound:pause(1))
		(if(GetSaveDisk(TRUE))
			(if(gPrintDlg)
				(send gPrintDlg:dispose())
			)
			= gameNum (Save:doit(@strDescBuf))
			(if(<> gameNum -1)
				= oldCursor (self:setCursor(gLoadingCursor 1))
				(if(not SaveGame(objectName gameNum @strDescBuf gVersion))
					Print(
						"Your save game disk is full. You must either "+
						"use another disk or save over an existing saved game."
						#font 0
						#button "OK" 1
					)
				)
				(self:setCursor(oldCursor HaveMouse()))
			)
  			GetSaveDisk(FALSE)
		)
  		(Sound:pause(hSound))
	)
	(method (changeScore addPoints)
		= gScore (+ gScore addPoints)
		(SL:doit())
	)
	(method (handleEvent pEvent)
		(if((send gRegions:handleEvent(pEvent)) or (send gLocales:handleEvent(pEvent)))
			return( (send pEvent:claimed) )
		)
		(if(script)
			(send script:handleEvent(pEvent))
		)
  		return( (send pEvent:claimed) )
	)
	(method (showMem)
		FormatPrint(
		    "Free Heap: %u Bytes\nLargest ptr: %u Bytes\nFreeHunk: %u KBytes\nLargest hunk: %u Bytes"
		    MemoryInfo(miFREEHEAP)
		    MemoryInfo(miLARGESTPTR)		    
		    (>> MemoryInfo(miFREEHUNK) 6)		    
		    MemoryInfo(miLARGESTHUNK)
		)
	)
	(method (setSpeed newSpeed)
		(var oldSpeed)
		= oldSpeed gSpeed
		= gSpeed newSpeed
		return(oldSpeed)
	)
	(method (setCursor csrNumber params)
		(var oldCursor)
		= oldCursor gCurrentCursor
		= gCurrentCursor csrNumber
		
		SetCursor(
		    csrNumber
		    rest params
		)
		
		return(oldCursor)
	)
	(method (checkAni)
		(var hCastMember)
		Animate((send gCast:elements) 0)
		Wait(0)
		Animate((send gCast:elements) 0)
		(while(> Wait(0) gCheckAniWait)
			= hCastMember (send gCast:firstTrue(#isExtra))
			(if(== hCastMember NULL) break)
			
			(send hCastMember:addToPic())
			Animate((send gCast:elements()) 0)
			(send gCast:eachElementDo(#delete))
  		)
	)
	(method (notify))
	(method (setScript newScript params)
		(if(script)
			(send script:dispose())
		)
		(if(newScript)
			(send newScript:init(self rest params))
		)
	)
	(method (cue)
		(if(script)
			(send script:cue())
		)
	)
	(method (wordFail aWord)
		(var temps[100])
		FormatPrint("I don't understand \"%s\"." aWord)
		return(FALSE)
	)
	(method (syntaxFail)
		Print("That doesn't appear to be a proper sentence.")
	)
	(method (semanticFail)
		Print("That sentence doesn't make sense.")
	)
	(method (pragmaFail)
		Print("You've left me responseless.")
	)
)
/******************************************************************************/
(class Rgn of Obj
	(properties
		script 0
		number 0
		timer 0
		keep 0
		initialized FALSE
	)
	(method (init)
		(if( not initialized )
			= initialized TRUE
			(if( not (send gRegions:contains(self)) )
				(send gRegions:addToEnd(self))
		    )
		    (super:init())
		)
	)
	(method (doit)
		(if(script)
			(send script:doit())
		)
	)
	(method (dispose)
		(send gRegions:delete(self))
		(if(IsObject(script))
			(send script:dispose())
		)
		(if(IsObject(timer))
			(send timer:dispose())
		)
		(send gSounds:eachElementDo(#clean self))
		DisposeScript(number)
	)
	(method (handleEvent pEvent)
		(if(script)
			(send script:handleEvent(pEvent))
		)
		return((send pEvent:claimed))
	)
	(method (setScript newScript params)
		(if(IsObject(script))
		    (send script:dispose())
		)
		(if(newScript)
			(send newScript:init(self rest params))
		)
	)
	(method (cue)
		(if(script)
			(send script:cue())
		)
	)
	(method (newRoom))
	(method (notify))
)
/******************************************************************************/
(class Rm of Rgn
	(properties
		script 0
		number 0
		timer 0
		keep 0
		initialized 0
		picture 0
		style $FFFF
		horizon 0
		controls 0
		north 0
		east 0
		south 0
		west 0
		curPic 0
		picAngle 0
		vanishingX 160
		vanishingY 35536
	)
	(method (init)
		= number gRoomNumberExit
		= controls _controls
		= gPicAngle picAngle
		(if(picture)
			(self:drawPic(picture))
		)
		(switch( (send (User:alterEgo):edgeHit) )
			(case EDGE_TOP
				(send (User:alterEgo):y(185))
			)
			(case EDGE_LEFT
		    	(send (User:alterEgo):x( (- 315 (send (User:alterEgo):xStep) ) ))
		    )
			(case EDGE_BOTTOM
		    	(send (User:alterEgo):y( (+ horizon (send (User:alterEgo):yStep)) ))
		    )
		    (case EDGE_RIGHT
		    	(send (User:alterEgo):x(4))
			)
		)
		(send (User:alterEgo):edgeHit(EDGE_NONE))
	)
	(method (doit)
		(var newRoomDir)
		(if(script)
			(send script:doit())
		)
		(switch(send (User:alterEgo):edgeHit())
			(case EDGE_TOP
		    	= newRoomDir north
			)
			(case EDGE_RIGHT
		    	= newRoomDir east
			)
			(case EDGE_BOTTOM
		    	= newRoomDir south
			)
			(case EDGE_LEFT
		    	= newRoomDir west
			)
			(default
				return
			)
		)
		(if(newRoomDir)
			(self:newRoom(newRoomDir))
		)
	)
	(method (dispose)
		(if(controls)
			(send controls:dispose())
		)
		(super:dispose())
	)
	(method (handleEvent pEvent)
		(if((super:handleEvent(pEvent)) or controls)
		    (send controls:handleEvent(pEvent))
		)
		return( (send pEvent:claimed) )
	)
	(method (newRoom newRoomNum)
		(send gRegions:
			delete(self)
			eachElementDo(#newRoom newRoomNum)
			addToFront(self)
		)
		= gRoomNumber newRoomNum
		(super:newRoom(newRoomNum))
	)
	(method (setRegions scriptNumbers)
		(var i, scrNum, hScript)
  		(for (= i 0) (< i paramTotal) (++i)
  			= scrNum scriptNumbers[i]
  			= hScript ScriptID(scrNum)
  			(send hScript:number(scrNum))
  			(send gRegions:add(hScript))
  			(if( not(send hScript:initialized) )
  				(send hScript:init())
		    )
		)
	)
	(method (setFeatures features)
		(var i)
		(for (= i 0) (< i paramTotal) (++i)
			(send gFeatures:add(features[i]))
		)
	)
	(method (setLocales scriptNumbers)
		(var i, scrNum, hScript)
		(for (= i 0) (< i paramTotal) (++i)
			= scrNum scriptNumbers[i]
			= hScript ScriptID(scrNum)
			(send hScript:number(scrNum))
			(send gLocales:add(hScript))
			(send hScript:init())
		)
	)
	(method (drawPic picNum picAni)
		(addToPics:dispose())
		= curPic picNum
		= gOverlay -1
		(if(== paramTotal 2)
			DrawPic(picNum picAni dpCLEAR gDefaultPalette)
		)(else
			(if(<> style -1)
				DrawPic(picNum style dpCLEAR gDefaultPalette)
			)(else
				DrawPic(picNum gDefaultPicAni dpCLEAR gDefaultPalette)
			)
		)
	)
	(method (overlay picNum picAni)
		= gOverlay picNum
		(if(== paramTotal 2)
		    DrawPic(picNum picAni dpNO_CLEAR gDefaultPalette)
		)(else
			(if(<> style -1)
		    	DrawPic(picNum style dpNO_CLEAR gDefaultPalette)
		    )(else
		    	DrawPic(picNum gDefaultPicAni dpNO_CLEAR gDefaultPalette)
		   	)
		)
	)
)
/******************************************************************************/
(class Locale of Obj
	(properties
		number 0
	)
	(method (dispose)
		(send gLocales:delete(self))
		DisposeScript(number)
	)
	(method (handleEvent pEvent)
		return((send pEvent:claimed))
	)
)
/******************************************************************************/
(class SL of Obj
	(properties
		state DISABLED
		code  NULL
	)
	(method (doit)
		(var strBuf[41])
		(if(code)
			(send code:doit(@strBuf))
		    (if(state)
		    	DrawStatus(@strBuf)
		    )(else
		    	DrawStatus(FALSE)
		    )
		)
	)
	(method (enable) 
		= state ENABLED
		(self:doit())
	)
	(method (disable)
		= state DISABLED
		(self:doit())
	)
)
/******************************************************************************/
(instance RU  of Code
	(method (doit param1)
		(var newSignal)
		(if((send param1:underBits))
			= newSignal (send param1:signal)
			= newSignal (| newSignal 1)
			= newSignal (& newSignal $FFFB)
			(send param1:
				underBits(0)
				signal(newSignal)
			)
		)
	)
)
/******************************************************************************/
(instance DNKR  of Code
	(properties)
	(method (doit param1)
		(if( not (send param1:keep) )  	
			(send param1:dispose())
		)
	)
)
/******************************************************************************/
(procedure (GetSaveDisk fSAVEDISK)
	(var btnPressed, temp1[40], temp41[40], temp81[40])
		   
	= btnPressed 1 // btnOK
	DeviceInfo(0 gVersion @temp1)
	DeviceInfo(@temp41)
	(if(DeviceInfo(2 @temp1 @temp41) and DeviceInfo(3 @temp41))
		(if(fSAVEDISK)
			Format(@temp81 "Please insert your SAVE GAME disk in drive %s." @temp41)
		)(else
			Format(@temp81 "Please insert your GAME disk in drive %s." @temp41)
		)
		DeviceInfo(4)
		(if(fSAVEDISK)
			= btnPressed Print(
				@temp81
				#font 0
				#button "OK" 1
				#button "Cancel" 0
				#button "Change Directory" 2
			)
		)(else
  			= btnPressed Print(
  				@temp81
  				#font 0
  				#button "OK" 1
  			)
		)
		(if(== btnPressed 2) // btnChangeDirectory
			SRChangeDirectory(gVersion)
		)
	)
	return(btnPressed)
)
/******************************************************************************/
