/*************************************************************************
 * SCI Script Compiler Header
 * By Brian Provinciano
 *************************************************************************/
(include "keys.sh") 
/*************************************************************************
 * General
 *************************************************************************/
(define TRUE		1)
(define FALSE		0)

(define ENABLED		1)
(define DISABLED	0)

(define NULL		0)

// Rects
(define rtTOP		0)
(define rtLEFT		1)
(define rtBOTTOM	2)
(define rtRIGHT		3)
/*************************************************************************
 * Colours
 *************************************************************************/
(define clBLACK			0)
(define clNAVY			1)
(define clGREEN			2)
(define clTEAL			3)
(define clMAROON		4)
(define clPURPLE		5)
(define clBROWN			6)
(define clSILVER		7)
(define clGREY			8)
(define clBLUE			9)
(define clLIME			10)
(define clCYAN			11)
(define clRED			12)
(define clFUCHSIA		13)
(define clYELLOW		14)
(define clWHITE			15)

(define clTRANSPARENT	-1)
/*************************************************************************
 * Resource types
 *************************************************************************/
(define rsVIEW   $80)
(define rsPIC    $81)
(define rsSCRIPT $82)
(define rsTEXT   $83)
(define rsSOUND  $84)
(define rsMEMORY $85)
(define rsVOCAB  $86)
(define rsFONT   $87)
(define rsCURSOR $88)
(define rsPATCH  $89)
/*************************************************************************
 * SCREENS
 *************************************************************************/
(define VISUAL                       1)
(define PRIORITY					 2)
(define CONTROL					     4)

(define VISUAL_PRIORITY              3)
(define VISUAL_CONTROL               5)
(define PRIORITY_CONTROL             6)

(define ALL_SCREENS  		         7)

(define ctlBLACK			         $0001)
(define ctlNAVY			             $0002)
(define ctlGREEN			         $0004)
(define ctlTEAL			             $0008)
(define ctlMAROON		             $0010)
(define ctlPURPLE		             $0020)
(define ctlBROWN			         $0040)
(define ctlSILVER		             $0080)
(define ctlGREY			             $0100)
(define ctlBLUE			             $0200)
(define ctlLIME			             $0400)
(define ctlCYAN			             $0800)
(define ctlRED			             $1000)
(define ctlFUCHSIA		             $2000)
(define ctlYELLOW		             $4000)
(define ctlWHITE			         $8000)
/*************************************************************************
 * DrawPic options 
 *************************************************************************/
(define dpOPEN_INSTANTLY 			-1)  // Display instantly 
(define dpOPEN_HCENTER 				0)   // horizontally open from center 
(define dpOPEN_VCENTER 				1)   // vertically open from center 
(define dpOPEN_RIGHT 				2)   // open from right 
(define dpOPEN_LEFT 				3)   // open from left 
(define dpOPEN_BOTTOM 				4)   // open from bottom 
(define dpOPEN_TOP 					5)   // open from top 
(define dpOPEN_EDGECENTER 			6)   // open from edges to center 
(define dpOPEN_CENTEREDGE 			7)   // open from center to edges 
(define dpOPEN_CHECKBOARD			8)   // open random checkboard 
(define dpCLOSEREOPEN_HCENTER		9)   // horizontally close to center, reopen from center 
(define dpCLOSEREOPEN_VCENTER		10)  // vertically close to center, reopen from center 
(define dpCLOSEREOPEN_RIGHT			11)  // close to right, reopen from right 
(define dpCLOSEREOPEN_LEFT			12)  // close to left, reopen from left 
(define dpCLOSEREOPEN_BOTTOM		13)  // close to bottom, reopen from bottom 
(define dpCLOSEREOPEN_TOP			14)  // close to top, reopen from top 
(define dpCLOSEREOPEN_EDGECENTER	15)  // close from center to edges, reopen from edges to center 
(define dpCLOSEREOPEN_CENTEREDGE	16)  // close from edges to center, reopen from center to edges 
(define dpCLOSEREOPEN_CHECKBOARD	17)  // close random checkboard, reopen 

(define dpCLEAR						1)   // Clear the screen before drawing
(define dpNO_CLEAR					0)   // Don't clear the screen before drawing
/*************************************************************************
 * NewWindow options 
 *************************************************************************/
(define nwNORMAL					0)   // border, no title 
(define nwTRANSPARENT				1)   // transparency 
(define nwNOFRAME					2)	 // window does not have a frame 
(define nwTRANSPARENT_NOFRAME		3)	 // transparency, window does not have a frame 
(define nwTITLE						4)	 // the window has a title 
(define nwNODRAW					8)  // don't draw anything 

(define nwON_TOP					-1)
/*************************************************************************
 * Display options 
 *************************************************************************/
(define dsCOORD						100) // 2 params, (X,Y) coord of where to write on the port. 
(define dsALIGN						101) // 1 param, -1, 0 or 1 (align right (-1), left (0) or center (1) 
(define dsCOLOR						102) // 1 param, set the text color. 
(define dsCOLOUR					102) // 1 param, set the text color. 
(define dsBACKGROUND				103) // 1 param, set the background color (-1 to draw text with transparent background) 
(define dsDISABLED					104) // 1 param, set the "gray text" flag (1 to draw disabled items) 
(define dsFONT						105) // 1 param, (resource number) set the font 
(define dsWIDTH						106) // 1 param, set the width of the text (the text wraps to fit in that width) 
(define dsSAVEPIXELS				107) // no param, set the "save under" flag, to save a copy of the pixels before writing the text (the handle to the saved pixels is returned) 
(define dsRESTOREPIXELS				108) // 1 param, (handle to stored pixels) restore under. With this command, the text and all other parameters are ignored. 

(define alRIGHT						-1)  // Align right
(define alLEFT						0)   // Align left
(define alCENTER					1)   // Align center
/*************************************************************************
 * Events
 *************************************************************************/
// Event types
(define evNULL						$0000) // Null event 
(define evMOUSEBUTTON				$0001) // Mouse button event 
(define evMOUSERELEASE				$0002) // Mouse button release event
(define evMOUSE				        $0003) // Mouse button event
(define evKEYBOARD					$0004) // Keyboard event 
(define evJOYSTICK					$0040) // Movement (joystick) event 
(define evSAID						$0080) // Said event 
(define evNOJOYSTICK				$8000) // no joystick event 

(define evMOUSEKEYBOARD				$0005) // Mouse/Keyboard event
(define evALL_EVENTS				$7FFF) // All events

// Event messages
//   These are the  set bits, you must and the  
//   message with these masks to check them.
(define emLEFT_BUTTON					1)
(define emRIGHT_BUTTON					3)
/*************************************************************************
 * File I/O
 *************************************************************************/
// open or fail: Try to open file, abort if not possible 
(define fOPENFAIL					0)   
// open or create: Try to open file, create it if it doesn't exist 
(define fOPENCREATE					1)   
// create: Create the file, destroying any content it might have had
(define fCREATE 					2)   
/*************************************************************************
 * DoSound
 *************************************************************************/

 #ifdef SCI_0
(define sndINIT						0)  
(define sndPLAY						1)  
(define sndNOP 						2)
(define sndDISPOSE					3)  
(define sndSET_SOUND				4)  
(define sndSTOP						5)
(define sndPAUSE					6)
(define sndRESUME					7)  
(define sndVOLUME					8)
(define sndUPDATE					9)  
(define sndFADE						10)  
(define sndCHECK_DRIVER				11)
(define sndSTOP_ALL					12)
#endif

#ifdef SCI_1_1
(define sndMASTER_VOLUME 0)
(define sndMUTE 1)
(define sndRESTORE 2)
(define sndGET_POLYPHONY 3)
(define sndGET_AUDIO_CAPABILITY 4)
(define sndSUSPEND 5)
(define sndINIT 6)
(define sndDISPOSE 7)
(define sndPLAY 8)
(define sndSTOP 9)
(define sndPAUSE 10)
(define sndFADE 11)
(define sndSET_HOLD 12)
(define sndDUMMY 13)
(define sndSET_VOLUME 14)
(define sndSET_PRIORITY 15)
(define sndSET_LOOP 16)
(define sndUPDATE_CUES 17)
(define sndSEND_MIDI 18)
(define sndGLOBAL_REVERB 19)
(define sndUPDATE 20)

(define fiOPEN 0)
(define fiCLOSE 1)
(define fiREAD 2)
(define fiWRITE 3)
(define fiUNLINK 4)
(define fiREAD_STRING 5)
(define fiWRITE_STRING 6)
(define fiSEEK 7)
(define fiFIND_FIRST 8)
(define fiFIND_NEXT 9)
(define fiEXISTS 10)
(define fiRENAME 11)

(define palSET_FROM_RESOURCE 1)
(define palSET_FLAG 2)
(define palUNSET_FLAG 3)
(define palSET_INTENSITY 4)
(define palFIND_COLOR 5)
(define palANIMATE 6)
(define palSAVE 7)
(define palRESTORE 8)

(define pvINIT 0)
(define pvREVERSE 1)
(define pvGET_CURRENT_STEP 2)
(define pvUNINIT 3)
(define pvCHANGE_TARGET 4)
(define pvCHANGE_TICKS 5)
(define pvPAUSE_RESUME 6)

(define msgGET 0)
(define msgNEXT 1)
(define msgSIZE 2)
(define msgREF_NOUN 3)
(define msgREF_VERB 4)
(define msgREF_COND 5)
(define msgPUSH 6)
(define msgPOP 7)
(define msgLAST_MESSAGE 8)
#endif  // SCI_1_1

// Sound statuses
(define ssSTOPPED                   0)
(define ssINITIALIZED               1)
(define ssPAUSED                    3)
/*************************************************************************
 * Controls
 *************************************************************************/
(define ctlBUTTON					1)  
(define ctlTEXT						2)  
(define ctlEDIT						3)
(define ctlICON						4) 
// ctl 5 does not exist 
(define ctlSELECTOR					6)  
/*************************************************************************
 * Time
 *************************************************************************/
(define gtTIME_ELAPSED				0)  
(define gtTIME_OF_DAY				1)  
/*************************************************************************
 * Memory
 *************************************************************************/
(define miFREEHEAP				    0)
(define miLARGESTPTR				1)
(define miLARGESTHUNK				2)
(define miFREEHUNK					3)  
/*************************************************************************
 * Shake Screen
 *************************************************************************/ 
(define ssUPDOWN					1) 
(define ssLEFTRIGHT					2)  
(define ssFULL_SHAKE				3)
/*************************************************************************
 * OnControl
 *************************************************************************/ 
(define ocVISUAL					1) 
(define ocPRIORITY					2)  
(define ocSPECIAL					4)
/*************************************************************************
 * Directions
 *************************************************************************/ 
(define CENTER						0)
(define UP							1)
(define UPRIGHT						2)
(define RIGHT						3)
(define DOWNRIGHT					4)
(define DOWN						5)
(define DOWNLEFT					6)
(define LEFT						7)
(define UPLEFT						8)

// Cycle Directions
(define cdFORWARD                   1)
(define cdNONE                      0)
(define cdBACKWARD                 -1)
/*************************************************************************
 * Edges
 *************************************************************************/ 
(define EDGE_NONE					0)
(define EDGE_TOP					1)
(define EDGE_RIGHT					2)
(define EDGE_BOTTOM					3)
(define EDGE_LEFT					4)
/*************************************************************************
 * Doors
 *************************************************************************/ 
(define DOOR_NULL					0)
(define DOOR_CUED					1)
(define DOOR_OPEN					2)
(define DOOR_CLOSED					3)
/*************************************************************************
 * On Controls
 *************************************************************************/
(define USE_POINT					1)
(define USE_RECT					0)
/*************************************************************************
 * String stuff
 *************************************************************************/
(define STRING_LESSTHAN				-1)
(define STRINGS_EQUAL				 0)
(define STRING_GREATER				 1)
/*************************************************************************
 * DeviceInfo
 *************************************************************************/
(define diGET_DEVICE                 0)
(define diGET_CURRENT_DEVICE         1)
(define diPATHS_EQUAL                2)
(define diIS_FLOPPY                  3)
/*************************************************************************
 * Joystick
 *************************************************************************/
(define jsCALL_DRIVER               $C)
/*************************************************************************
 * Graph
 *************************************************************************/
(define grGET_COLOURS               2)
(define grDRAW_LINE                 4)
(define grSAVE_BOX                  7)
(define grRESTORE_BOX               8)
(define grFILL_BOX_BACKGROUND       9)
(define grFILL_BOX_FOREGROUND       10)
(define grFILL_BOX                  11)
(define grUPDATE_BOX                12)
(define grREDRAW_BOX                13)
(define grADJUST_PRIORITY           14)
/*************************************************************************
 * SetMenu
 *************************************************************************/
(define smMENU_NOSAID                32)
(define smMENU_SAID                 109)  // aSaid
(define smMENU_TEXT                 110)  // aString
(define smMENU_SHORTCUTKEY          111)  // aChar
(define smMENU_ENABLE               112)  // aBool
(define smMENU_113                  113)
/*************************************************************************
 * SetCursor
 *************************************************************************/
(define SET_CURSOR_VISIBLE          1)
/*************************************************************************/


