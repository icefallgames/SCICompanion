.. Main.sc

.. default - domain::js

.. include:: /includes/standard.rst

=======
Main.sc
=======


This is the main game script. It contains the main game class, all the global variables, and
a number of useful procedures.

In addition to the above, it contains the icon instances for the icons
in the control bar. It also contains the crucial default Messager
and its findTalker method (used for mapping talker numbers to a Talker or Narrator instance).


Classes
==========

:class:`SQ5` of Game

Public Procedures
=================

:func:`Btest`

:func:`Bset`

:func:`Bclear`

:func:`RestorePreviousHandsOn`

:func:`IsObjectOnControl`

:func:`SetUpEgo`

:func:`AimToward`

:func:`Die`

:func:`AddToScore`

:func:`ScoreFlag`

:func:`HideStatus`

:func:`DebugPrint`

:func:`AddPolygonsToRoom`

:func:`CreateNewPolygon`

Global variables 
=================

:

====================== ===================================================================
Property               Description                                                        
====================== ===================================================================
gEgo                   The object for the player's ego.                                   
gGame                  The game object.                                                   
gRoom                  The current room object.                                           
global3                Unused                                                             
gQuitGame                                                                                 
gCast                                                                                     
gRegions               The current regions.                                               
gTimers                The current timers.                                                
gSounds                The current sounds.                                                
gInv                   The inventory.                                                     
gAddToPics                                                                                
gRoomNumber            The current room number                                            
gPreviousRoomNumber                                                                       
gNewRoomNumber                                                                            
gDebugOnNextRoom                                                                          
gScore                 The player's current score.                                        
gMaxScore              The maximum score.                                                 
gTextCode                                                                                 
gCuees                                                                                    
gCursorNumber                                                                             
gNormalCursor                                                                             
gWaitCursor                                                                               
gFont                  Main font number.                                                  
gSmallFont             Small font number.                                                 
gPEvent                The current event.                                                 
gDialog                The current Print dialog.                                          
gBigFont               Big font number.                                                   
gVersion                                                                                  
gSaveDir                                                                                  
gPicAngle                                                                                 
gFeatures                                                                                 
gUseSortedFeatures                                                                        
gPicNumber                                                                                
gDoMotionCue                                                                              
gWindow                                                                                   
global39               Unused                                                             
global40               Unused                                                             
gOldPort                                                                                  
gDebugFilename         debug filename                                                     
gGameControls          The main GameControls class.                                       
gFeatureInit           Code that initializes all features.                                
gDoVerbCode                                                                               
gApproachCode                                                                             
gEgoUseObstacles       Default Motion type for ego (0: MoveTo, 1: PolyPath, ...)          
gIconBar                                                                                  
gPEventX               Current event's x value.                                           
gPEventY               Current event's y value.                                           
gOldKH                                                                                    
gOldMH                                                                                    
gOldDH                                                                                    
gPseudoMouse                                                                              
gTheDoits                                                                                 
gEatTheMice            Number of ticks before we mouse                                    
gUser                                                                                     
gSyncBias              Something to do with lip-sync.                                     
gTheSync                                                                                  
global83               Something to do with audio narration.                              
gFastCast                                                                                 
gInputFont                                                                                
gTickOffset            Something to do with time (ticks per frame?)                       
gGameTime                                                                                 
gNarrator              Default Narrator.                                                  
gMessageType           Talker flags: 0x1 (text) and 0x2 (audio).                          
gMessager                                                                                 
gPrints                                                                                   
gWalkHandler                                                                              
gTextReadSpeed                                                                            
gAltPolyList                                                                              
gColorDepth                                                                               
gPolyphony                                                                                
gStopGroop                                                                                
global107                                                                                 
gCurrentIcon                                                                              
gGUserCanControl                                                                          
gGUserCanInput                                                                            
gCheckedIcons                                                                             
gState                                                                                    
gNewSpeakWindow                                                                           
gWindow2                                                                                  
gDeathReason                                                                              
gMusic1                                                                                   
gDongle                This variable CAN'T MOVE                                           
gMusic2                                                                                   
gCurrentTalkerNumber                                                                      
gGEgoMoveSpeed                                                                            
gColorWindowForeground                                                                    
gColorWindowBackground                                                                    
gLowlightColor                                                                            
gDefaultEgoView        The default view resource for the ego                              
gRegister                                                                                 
gFlags                 Start of bit set. Room for 14 x 16 = 224 flags.                    
gEdgeDistance          Margin around screen to make it easier to walk the ego to the edge.
gDebugOut                                                                                 
====================== ===================================================================

