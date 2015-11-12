.. Kernel Functions

.. default - domain::js

.. include:: /includes/standard.rst

================
Kernel Functions
================


Kernel functions are routines built into the interpreter that can be called from scripts.
They are essential as they do the graphic drawing, window handling, text printing, file I / O, and everything else you need.
The number of kernel functions vary from intepreter versions, but in the interpreter for the SCI0 template game, there are 114. In the SCI1.1
interpreter there are 136.

.. toctree::
	:maxdepth: 1

	Kernels/Abs
	Kernels/AddAfter
	Kernels/AddMenu
	Kernels/AddToEnd
	Kernels/AddToFront
	Kernels/AddToPic
	Kernels/Animate
	Kernels/AssertPalette
	Kernels/ATan
	Kernels/AvoidPath
	Kernels/BaseSetter
	Kernels/CanBeHere
	Kernels/CantBeHere
	Kernels/CelHigh
	Kernels/CelWide
	Kernels/CheckFreeSpace
	Kernels/CheckSaveGame
	Kernels/Clone
	Kernels/CoordPri
	Kernels/CosDiv
	Kernels/CosMult
	Kernels/DbugStr
	Kernels/DeleteKey
	Kernels/DeviceInfo
	Kernels/DirLoop
	Kernels/Display
	Kernels/DisposeClone
	Kernels/DisposeList
	Kernels/DisposeScript
	Kernels/DisposeWindow
	Kernels/DoAudio
	Kernels/DoAvoider
	Kernels/DoBresen
	Kernels/DoSound
	Kernels/DoSync
	Kernels/DrawCel
	Kernels/DrawControl
	Kernels/DrawMenuBar
	Kernels/DrawPic
	Kernels/DrawStatus
	Kernels/EditControl
	Kernels/EmptyList
	Kernels/FClose
	Kernels/FGets
	Kernels/FileIO
	Kernels/FindKey
	Kernels/FirstNode
	Kernels/FlushResources
	Kernels/FOpen
	Kernels/Format
	Kernels/FPuts
	Kernels/GameIsRestarting
	Kernels/GetAngle
	Kernels/GetCWD
	Kernels/GetDistance
	Kernels/GetEvent
	Kernels/GetFarText
	Kernels/GetMenu
	Kernels/GetPort
	Kernels/GetSaveDir
	Kernels/GetSaveFiles
	Kernels/GetTime
	Kernels/GlobalToLocal
	Kernels/Graph
	Kernels/HaveMouse
	Kernels/HiliteControl
	Kernels/InitBresen
	Kernels/InspectObj
	Kernels/IsItSkip
	Kernels/IsObject
	Kernels/Joystick
	Kernels/LastNode
	Kernels/ListOps
	Kernels/Load
	Kernels/LocalToGlobal
	Kernels/Lock
	Kernels/MapKeyToDir
	Kernels/Memory
	Kernels/MemoryInfo
	Kernels/MemorySegment
	Kernels/MenuSelect
	Kernels/MergePoly
	Kernels/Message
	Kernels/NewList
	Kernels/NewNode
	Kernels/NewWindow
	Kernels/NextNode
	Kernels/NodeValue
	Kernels/NumCels
	Kernels/NumLoops
	Kernels/OnControl
	Kernels/Palette
	Kernels/PalVary
	Kernels/Parse
	Kernels/PicNotValid
	Kernels/Platform
	Kernels/PlayBack
	Kernels/PrevNode
	Kernels/Profiler
	Kernels/Random
	Kernels/ReadNumber
	Kernels/Record
	Kernels/ResCheck
	Kernels/RespondsTo
	Kernels/RestartGame
	Kernels/RestoreGame
	Kernels/Said
	Kernels/SaveGame
	Kernels/ScriptID
	Kernels/SetCursor
	Kernels/SetDebug
	Kernels/SetJump
	Kernels/SetMenu
	Kernels/SetNowSeen
	Kernels/SetPort
	Kernels/SetQuitStr
	Kernels/SetSynonyms
	Kernels/SetVideoMode
	Kernels/ShakeScreen
	Kernels/ShiftScreen
	Kernels/Show
	Kernels/ShowFree
	Kernels/ShowMovie
	Kernels/ShowObjs
	Kernels/ShowSends
	Kernels/SinDiv
	Kernels/SinMult
	Kernels/Sort
	Kernels/Sqrt
	Kernels/StackUsage
	Kernels/StrAt
	Kernels/StrCat
	Kernels/StrCmp
	Kernels/StrCpy
	Kernels/StrEnd
	Kernels/StrLen
	Kernels/StrSplit
	Kernels/TextColors
	Kernels/TextFonts
	Kernels/TextSize
	Kernels/UnLoad
	Kernels/ValidPath
	Kernels/Wait

