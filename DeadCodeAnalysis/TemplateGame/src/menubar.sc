/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 menubar.sc
 Contains the customized Menubar class. This is the script you modify if you 
 want to customize the menu.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script MENUBAR_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "gauge")
(use "sound")
(use "user")
/******************************************************************************/
(class TheMenuBar of MenuBar
	(properties
		state 0
	)
	(method (init)
		AddMenu(
			"  "
				"About Template`^a:"+
				"Help`#1"
		)
		AddMenu(
			" File "
				"Restart Game`#9:"+
				"Save Game`#5:"+
				"Restore Game`#7:"+
				"--! :"+
				"Quit`^q"
		)
		AddMenu(
			" Action "
				"Pause Game`^p:"+
				"Inventory`^I:"+
				"Retype`#3:"+
				"--! :"+
				"Colors`^c"
		)
		AddMenu(
			" Speed "
				"Change...`^s:"+
				"--!:"+
				"Faster`+:"+
				"Normal`=:"+
				"Slower`-"
		)
		(if(DoSound(sndSET_SOUND))
			AddMenu(
				" Sound "
					"Volume...`^v:"+
					"Turn Off`#2=1"
			)
		)(else
			AddMenu(
				" Sound "
					"Volume...`^v:"+
					"Turn On`#2=1"
			)
		)
		(if(< Graph(grGET_COLOURS) 9)
			SetMenu(MENU_COLOURS 32 0)
		)(else
			SetMenu(MENU_COLOURS smMENU_SAID '/color')
		)
		SetMenu(MENU_SAVE		smMENU_SAID 'save[/game]')
		SetMenu(MENU_RESTORE	smMENU_SAID 'restore[/game]')
		SetMenu(MENU_RESTART	smMENU_SAID 'restart[/game]')
		SetMenu(MENU_QUIT		smMENU_SAID 'done[/game]')
		SetMenu(MENU_PAUSE		smMENU_SAID 'delay[/game]')
		SetMenu(MENU_INVENTORY	smMENU_SAID 'all')
	)
	(method (handleEvent pEvent)
		(var menuItem, hGauge, newSpeed, newVolume, wndCol, wndBack, hPause)
		= menuItem (super:handleEvent(pEvent))
		(switch(menuItem)
			(case MENU_ABOUT
				Print(
					"       Template Game\n"+
					" By Brian Provinciano"
					#title "About"
				)
			)
			(case MENU_HELP
				Print(
					"<Put your how to play stuff here>"
					#title "How To Play"
				)
			)
			(case MENU_RESTART
				(if(Print(
					  "Are you serious? You really want to start all the way back at the beginning again?"
					  #title "Restart"
					  #font gDefaultFont
					  #button "Restart" 1
					  #button " Oops " 0
					)
				   )
					(send gGame:restart())						
				)
			)
			(case MENU_RESTORE   
				(send gGame:restore())
			)
			(case MENU_SAVE		  		    
				(send gGame:save())
			)
			(case MENU_QUIT		  	
				(if(Print(
					  "Do you really want to quit?"
					  #title "Quit"
					  #font gDefaultFont
					  #button " Quit " 1
					  #button " Oops " 0
					)
				   )
						= gQuitGame TRUE					
				)
			)
			(case MENU_PAUSE
				= hPause (Sound:pause())
 				Print("Game Paused")
 				(Sound:pause(hPause))
			)
			(case MENU_INVENTORY
				(if(PrintCantDoThat($400))
					(send gInv:showSelf(gEgo))
		    	)
			)
			(case MENU_RETYPE
				(send pEvent:
					claimed(FALSE)
					type(evKEYBOARD)
					message( (User:echo) )
				)
			)
			(case MENU_COLOURS
				= wndCol 16
				(while(>u wndCol 15 and <> wndCol -1)
					= wndCol GetNumber("New Text Color: (0-15)")
				)
				(if(<> wndCol -1)
					= wndBack 16
					(while(<> wndBack -1 and >u wndBack 15 or == wndCol wndBack)
						= wndBack GetNumber("New Background Color: (0-15)")
					)
					(if(<> wndBack -1)
						= gWndColor wndCol
						= gWndBack wndBack
						(send gTheWindow:
							color(gWndColor)
							back(gWndBack)
						)
					)
				)
			)
			(case MENU_CHANGESPEED
				= hGauge (Gauge:new())
				= newSpeed
					(send hGauge:
						text("Game Speed")
						description("Use the mouse or the left and right arrow keys to select the game speed.")
						higher("Faster")
						lower("Slower")
						normal(NORMAL_SPEED)
						doit(- 15 gSpeed)
					)
				(send gGame:setSpeed(- 15 newSpeed))
				DisposeScript(GAUGE_SCRIPT)
			)
			(case MENU_FASTERSPEED
				(if(gSpeed)
					(send gGame:setSpeed(--gSpeed))
				)
			)
			(case MENU_NORMALSPEED
				(if(gSpeed)
					(send gGame:setSpeed(12))
				)
			)
			(case MENU_SLOWERSPEED
				(if(< gSpeed 15)
					(send gGame:setSpeed(++gSpeed))
				)
			)
			(case MENU_VOLUME
				= hGauge (Gauge:new())
				= newVolume
					(send hGauge:
						text("Sound Volume")
						description("Use the mouse or the left and right arrow keys to adjust the volume.")
						higher("Louder")
						lower("Softer")
						normal(15)
						doit(DoSound(sndVOLUME newVolume))
					)
				DoSound(sndVOLUME newVolume)
				DisposeScript(GAUGE_SCRIPT)
			)
			(case MENU_TOGGLESOUND
				ToggleSound()
			)
		)
	)
)
/******************************************************************************/
(procedure public (ToggleSound)
	(var SOUND_OFF)
	= SOUND_OFF DoSound(sndSET_SOUND)
	= SOUND_OFF DoSound( sndSET_SOUND not(SOUND_OFF) )
	(if(SOUND_OFF)
		SetMenu(MENU_TOGGLESOUND smMENU_TEXT "Turn On")
	)(else
		SetMenu(MENU_TOGGLESOUND smMENU_TEXT "Turn Off")
	)
)
/******************************************************************************/
