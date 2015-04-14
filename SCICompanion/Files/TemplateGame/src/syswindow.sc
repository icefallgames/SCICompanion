/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 syswindow.sc
 Contains the base class for windows, as well as the Save/Restore dialogs.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script SYSWINDOW_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "obj")
/******************************************************************************/
(local
	hControl
	local1
	local2
	local3
	SRDialogType
	local5[4]
	local9
	localA[4] = (0 0 0 $08)
)
(string xs[200])
/******************************************************************************/
(class SysWindow of Obj
	(properties
		top 0
		left 0
		bottom 0
		right 0
		color clBLACK
		back clWHITE
		priority -1
		window 0
		type 0
		title 0
		brTop 0
		brLeft 0
		brBottom 190
		brRight 320
	)
	(method (dispose)
		(if(window)
			DisposeWindow(window)
			= window NULL
		)
		(super:dispose())
	)
	(method (open)
		= window NewWindow(
			top left bottom right
		    title
		    type
		    priority
		    color
		    back
		)
	)
)
/******************************************************************************/
(class SRDialog of Dialog
	(properties
		elements 0
		size 0
		text 0
		window 0
		theItem 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		time 0
		busy 0
		seconds 0
		lastSeconds 0
	)
	(method (init strDescription strBuf strPtrs)
		= window SysWindow
		= nsBottom 0
		= local2 GetSaveFiles( (send gGame:name) strBuf strPtrs )

		(if(== local2 -1)
			return(0)
		)
		
		= local5[0] "Restore"
		= local5[1] "  Save  "
		= local5[2] "Replace"
		= local5[3] local5[2]
		
		= local9 "Select the game you would like to restore."
		= localA[0] "Type the description of this saved game."
		
		= localA[1] 
				"This directory/disk can hold no more saved games. "+
				"You must replace one of your saved games or use Change "+
				"Directory to save on a different directory/disk."
		= localA[2] localA[1]  

		
		= SRDialogType IsSaveDialog()
		(if(== SRDialogType 1)
		 	(editI:
		 		text(StrCpy(strDescription strBuf))
		 		font(gSaveRestoreFont)
		 		setSize()
		 		moveTo(4 4)
		 	)
			(self:
				add(editI)
				setSize()
			)
		)
		(selectorI:
			text(strBuf)
			font(gSaveRestoreFont)
			setSize()
			moveTo(4 (+ nsBottom 4))
			state(2)
		)
		= local1 (+ (selectorI:nsRight) 4)
		(okI:
			text(local5[SRDialogType])
			setSize()
			moveTo(local1 (selectorI:nsTop))
		)

		(if(== SRDialogType 3)
		    (okI:state(0)) 
		)(else
		    (okI:state(3)) 
 		)
		(cancelI:
			setSize()
			moveTo(local1 (+ (okI:nsBottom) 4))
			state( & (cancelI:state) $FFF7 )
		)
		(changeDirI:
			setSize()
			moveTo(local1 (+ (cancelI:nsBottom) 4))
			state(& (changeDirI:state) $FFF7)
		)
		(self:
			add(selectorI okI cancelI changeDirI)
			setSize()
		)
		(textI:
			text(local9[SRDialogType])
			setSize(- (- nsRight nsLeft) 8)
			moveTo(4 4)
		)
		= local1 (+ (textI:nsBottom) 4)
		(self:
			eachElementDo(#move 0 local1)
		)
		(self:
			add(textI)
			setSize()
			center()
			open(nwTITLE 15)
		)
		return(1)
	)
	(method (doit strDescription)
		(var temp0, temp1, temp2, temp3, temp4[360], temp365[20], temp386[20])
		(if(== self Restore and paramTotal and strDescription)
			= temp1 FOpen( Format(@temp386 "%ssg.dir" (send gGame:name)) )
			(if(== temp1 -1)
				return
			)
			FClose(temp1)
		)
		(if(not (self:init(strDescription @temp4 @temp365)) )
			return(-1)
		)
		(while(1)
			(switch(SRDialogType)
				(case 0
					(if(local2)
			    		= hControl okI
			    	)
				)
				(case 1
					= hControl editI
				)
				(case 2
					= hControl okI
				)
				(default
					= hControl changeDirI
				)
			)
			= local1 (super:doit(hControl))
			= local3 (selectorI:indexOf( (selectorI:cursor) ))
			= temp3 (* local3 18)
			(if((== local1 changeDirI) and SRChangeDirectory(gSaveDirPtr))
		    	= local2 GetSaveFiles((send gGame:name) @temp4 @temp365)
		    	(if(== local2 -1)
		    		= temp2 -1
		    		break
		    	)
		    	= temp0 SRDialogType
		    	= SRDialogType IsSaveDialog()
		    	(if(== SRDialogType 0)
		    	)(else
		    		(if(== SRDialogType temp0)
						(if( (self:contains(editI)) )
		    				(editI:
								cursor(StrLen(StrCpy(strDescription @temp4)))
								draw()
							)
						)
					)(else
						(self:
							dispose()
							init(strDescription @temp4 @temp365)
						)
		    		)
				)
				(selectorI:draw())
		    )(else
		    	(if( (== SRDialogType 2) and (== local1 okI) )
					(if(GetReplaceName:doit(StrCpy(strDescription @temp4[temp3])))
						= temp2 temp365[local3]
						break
					)
				)(else
					(if((== SRDialogType 1) and (== local1 okI) or (== local1 editI))
							(if(== StrLen(strDescription) 0)
								Print("You must type a description for the game." #font 0)
							)(else
  								= temp2  -1
  								= local1 0
  								(while(< local1 local2)
  									(if(not(= temp2 StrCmp( strDescription @temp4[* local1 18] )))
  										break
  									)
  									++local1
  								)  	
  								(if(not temp2)
  									= temp2 temp365[local1]
  								)(else
  									(if(== local2 20)
  										= temp2 temp365[local3]
  									)(else
  										= temp2 local2
  									)
  								)
  								break
  							)
  					)(else
  						(if(== local1 okI)
  							= temp2 temp365[local3]
  							break
  						)
  						(if( (== local1 0) or (== local1 cancelI) )
  							= temp2 -1
  							break
  						)
  						(if(== SRDialogType 1)
		   					(editI:
		    					cursor( StrLen(StrCpy(strDescription @temp4[temp3])) )
								draw()
							)
						)
					)
				)
			)
		)
  		(self:dispose())
  		return(temp2)
	)
)
/******************************************************************************/
(class Restore of SRDialog
	(properties
		elements 0
		size 0
		text "Restore a Game"
		window 0
		theItem 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		time 0
		busy 0
		seconds 0
		lastSeconds 0
	)
)
/******************************************************************************/
(class Save of SRDialog
	(properties
		elements 0
		size 0
		text "Save a Game"
		window 0
		theItem 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		time 0
		busy 0
		seconds 0
		lastSeconds 0
	)
)
/******************************************************************************/
(instance GetReplaceName  of Dialog
	(properties)
	(method (doit strDescription)
		(var temp0)
		= window SysWindow
		(text1:
			setSize()
			moveTo(4 4)
		)
		(self:
			add(text1)
			setSize()
		)
		(oldName:
			text(strDescription)
			font(gSaveRestoreFont)
			setSize()
			moveTo(4 nsBottom)
		)
		
		(self:
			add(oldName)
			setSize()
		)
		
		(text2:
			setSize()
			moveTo(4 nsBottom)
		)
		(self:
			add(text2)
			setSize()
		)
		
		(newName:
			text(strDescription)
			font(gSaveRestoreFont)
			setSize()
			moveTo(4 nsBottom)
		)
		(self:
			add(newName)
			setSize()
		)
		(button1:
			nsLeft(0)
			nsTop(0)
			setSize()
		)
		
		(button2:
			nsLeft(0)
			nsTop(0)
			setSize()
		)
		
		(button2:
			moveTo((- nsRight (+ (button2:nsRight) 4)) nsBottom)
		)
		(button1:
			moveTo(
				(- (button2:nsLeft) (+ (button1:nsRight) 4))
				nsBottom
			)
		)
		(self:
			add(button1 button2)
			setSize()
			center()
			open(0 15)
		)
		= temp0 (super:doit(newName))
		(self:dispose())
		(if(not StrLen(strDescription))
			Print("You must type a description for the game." #font 0)
		    = temp0 0
		)
		(if(== temp0 newName or == temp0 button1)
			return(TRUE)
		)
		return(FALSE)
	)
)
/******************************************************************************/
(instance selectorI  of DSelector
	(properties
		type 6
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		font 0
		x 36
		y 8
		text 0
		cursor 0
		lsTop 0
		mark 0
	)
)
/******************************************************************************/
(instance editI  of DEdit
	(properties
		type 3
		state 1
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text 0
		font 0
		max 35
		cursor 0
	)
)
/******************************************************************************/
(instance okI  of DButton
	(properties)
)
/******************************************************************************/
(instance cancelI  of DButton
	(properties
		type 1
		state 3
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text " Cancel "
		font 0
	)
)
/******************************************************************************/
(instance changeDirI  of DButton
	(properties
		type 1
		state 3
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text "Change\nDirectory"
		font 0
	)
)
/******************************************************************************/
(instance textI  of DText
	(properties
		type 2
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text 0
		font 0
		mode 0
	)
)
/******************************************************************************/
(instance text1  of DText
	(properties
		type 2
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text "Replace"
		font 0
		mode 0
	)
)
/******************************************************************************/
(instance text2  of DText
	(properties
		type 2
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text "with:"
		font 0
		mode 0
	)
)
/******************************************************************************/
(instance oldName  of DText
	(properties)
)
/******************************************************************************/
(instance newName  of DEdit
	(properties
		type 3
		state 1
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text 0
		font 0
		max 35
		cursor 0
	)
)
/******************************************************************************/
(instance button1  of DButton
	(properties
		type 1
		state 3
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text "Replace"
		font 0
	)
)
/******************************************************************************/
(instance button2  of DButton
	(properties
		type 1
		state 3
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text "Cancel"
		font 0
	)
)
/******************************************************************************/
(procedure (IsSaveDialog) of SRDialog
	(if(== self Restore)
		return(0)
	)(else
		(if(CheckForFreeSpace())
		    return(1)
		)(else
			(if(local2)
		    	return(2)
			)(else
		    	return(3)
		    )
		)
	)
)
/******************************************************************************/
(procedure public (SRChangeDirectory strDescription) 
	(var temp0, temp1[30], temp34[30])
	(while(1)
		= temp0 Print(
			"New save-game directory:"
			#font 0
			#edit StrCpy(@temp1 strDescription) $1D
			#button "OK" 1
			#button "Cancel" 0
		)
		(if(not temp0)
			return 0
		)
		(if(not StrLen(@temp1))
			GetCWD(@temp1)
		)
		(if(ValidPath(@temp1))
			StrCpy(strDescription @temp1)
			return(1)
		)(else
		    Print(
			   Format(@temp34 "%s\nis not a valid directory" @temp1)
			   #font 0
			)
		)
	)
)
/******************************************************************************/
(procedure (CheckForFreeSpace)
	(if(< local2 20)
		CheckFreeSpace(gSaveDirPtr)
	)
)
/******************************************************************************/
