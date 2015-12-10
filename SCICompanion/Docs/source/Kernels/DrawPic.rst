.. DrawPic

.. default - domain::js

.. include:: /includes/standard.rst

================
DrawPic (Kernel)
================

.. function:: DrawPic(picNum [animation clear defaultPalette])

	
	Draws a picture on the screen. The last three parameters are optional.
	If specified, they can define how the picture is displayed.
	If unspecified, the last used (or default) parameters are used.
	
	:param number picNum: The number of the pic resource that should be drawn.
	:param number animation: A value indicating the type of animation. See below.
	:param boolean clear: (dpCLEAR or dpNOCLEAR) If dpCLEAR, the screen will be cleared before drawing the picture. 
	:param number defaultPalette: **SCI0 only**. specifies the default palette to be used for drawing it (0-3). The palettes are contained in the pic files and can be edited with SCI Studio's picture editor. This is useful for displaying your picture in day/night colours.
	
	==========================  ============================================================
	SCI0
	----------------------------------------------------------------------------------------
	Flag                        Description
	==========================  ============================================================
	dpOPEN_INSTANTLY            display instantly
	dpOPEN_HCENTER              horizontally open from center
	dpOPEN_VCENTER              vertically open from center
	dpOPEN_RIGHT                open from right
	dpOPEN_LEFT                 open from left
	dpOPEN_BOTTOM               open from bottom
	dpOPEN_TOP                  open from top
	dpOPEN_EDGECENTER           open from edges to center
	dpOPEN_CENTEREDGE           open from center to edges
	dpOPEN_CHECKBOARD           open random checkerboard
	dpCLOSEREOPEN_HCENTER       horizontally close to center, reopen from center
	dpCLOSEREOPEN_VCENTER       vertically close to center, reopen from center 
	dpCLOSEREOPEN_RIGHT         close to right, reopen from right 
	dpCLOSEREOPEN_LEFT          close to left, reopen from left 
	dpCLOSEREOPEN_BOTTOM        close to bottom, reopen from bottom
	dpCLOSEREOPEN_TOP           close to top, reopen from top 
	dpCLOSEREOPEN_EDGECENTER    close from center to edges, reopen from edges to center 
	dpCLOSEREOPEN_CENTEREDGE    close from edges to center, reopen from center to edges
	dpCLOSEREOPEN_CHECKBOARD    close random checkboard, reopen 
	==========================  ============================================================	
	
	In SCI1.1, it can be set to one of the following values:
	
	==========================  ============================================================
	SCI1.1
	----------------------------------------------------------------------------------------
	Flag                        Description
	==========================  ============================================================
	dpOPEN_INSTANTLY            display instantly
	dpOPEN_HCENTER              horizontally open from center
	dpOPEN_VCENTER              vertically open from center
	dpOPEN_RIGHT                open from right
	dpOPEN_LEFT                 open from left
	dpOPEN_BOTTOM               open from bottom
	dpOPEN_TOP                  open from top
	dpOPEN_EDGECENTER           open from edges to center
	dpOPEN_CENTEREDGE           open from center to edges
	dpOPEN_CHECKBOARD           open random checkerboard
	dpOPEN_PIXELATION           pixelated transition
	dpOPEN_FADEPALETTE          fade the palette to black
	dpOPEN_SCROLL_RIGHT         scroll to the right
	dpOPEN_SCROLL_LEFT          scroll to the left
	dpOPEN_SCROLL_UP            scroll up
	dpOPEN_SCROLL_DOWN          scroll down
	dpOPEN_NO_TRANSITION        straight cut.
	dpANIMATION_BLACKOUT        add this flag when transitioning between different palettes
	==========================  ============================================================	
	
	Since in SCI1.1, each room might be using a different palette, it can be useful to completely transition off
	the previous screen before transitioning to the new screen. This is what dpANIMATION_BLACKOUT is used for.
	
	Example::
	
		(DrawPic 100 dpCLOSEREOPEN_EDGECENTER dpCLEAR)


