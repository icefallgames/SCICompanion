(version 2)
(include "sci.sh")
(exports
    0 debugHandler
)
(use "Main")
(use "Controls")
(use "Print")
(use "ClickMenu")
(use "FeatureWriter")
(use "CueObj")
(use "SysWindow")
(use "User")
(use "View")
(use "Obj")
(script 10)


(local
    local0[27]

)
(procedure (localproc_0052)
    (if (IsOneOf((send gRoom:style) 11 12 13 14))
        (send gRoom:
            drawPic((send gRoom:picture) 100)
            style(100)
        )
    )
)


(instance public debugHandler of Feature
    (properties)

    (method (init)
        (super:init())
        (send gOldMH:addToFront(self))
        (send gOldKH:addToFront(self))
    )


    (method (dispose)
        (send gOldMH:delete(self))
        (send gOldKH:delete(self))
        (super:dispose())
        DisposeScript(10)
    )


    (method (handleEvent pEvent)
        (var temp0[160], temp160, newEvent, gOldCastFirst, theGFont, temp164, temp165, temp166, temp167, temp168, temp169, temp170, temp171, temp172, userAlterEgo, temp174, temp175, temp176, temp177, temp178)
        return 
            (switch ((send pEvent:type))
                (case evKEYBOARD
                    (send pEvent:claimed(1))
                    (switch ((send pEvent:message))
                        (case KEY_ALT_a
                        	// Show cast
                            = gOldCastFirst (send gOldCast:first())
                            (while (gOldCastFirst)
                                = temp164 NodeValue(gOldCastFirst)
                                Format(@temp0 10 1 (send ((send temp164:{-super-})):name) (send temp164:view) (send temp164:loop) (send temp164:cel) (send temp164:x) (send temp164:y) (send temp164:z) (send temp164:heading) (send temp164:priority) (send temp164:signal) 
                                    (if ((send temp164:isKindOf(Actor)))
                                        (send temp164:illegalBits)
                                    )(else
                                        -1
                                    )
									  )
                                (if (not (Print:
                                    addText(@temp0 CelWide((send temp164:view) (send temp164:loop) (send temp164:cel)) 0)
                                    window(SysWindow)
                                    addTitle((send temp164:name))
                                    addIcon((send temp164:view) (send temp164:loop) (send temp164:cel) 0 0)
                                    init()
										 )
									)
                                    break
                                )
                                = gOldCastFirst (send gOldCast:next(gOldCastFirst))
                            )
                        )
                        (case KEY_ALT_b
                        	// Polygon editor
                            (PolyEdit:doit())
                        )
                        (case KEY_ALT_y
                        	// Show polygons
                        	(= temp160 (send gRoom:obstacles))
							(if (temp160)
                        		(send temp160:eachElementDo(#perform drawPoly))
								Graph(grUPDATE_BOX 0 0 190 320 VISUAL)
							)
						)
                        (case KEY_ALT_c
                        	// Control screen
                            localproc_0052()
                            Show(CONTROL)
                        )
                        (case KEY_ALT_e
                        	// Show ego info
                            Format(@temp0 10 2 (send gEgo:name) (send gEgo:view) (send gEgo:loop) (send gEgo:cel) (send gEgo:x) (send gEgo:y) (send gEgo:z) (send gEgo:heading) (send gEgo:priority) (send gEgo:signal) (send gEgo:illegalBits) (send gEgo:onControl()) (send gEgo:onControl(1)))
                            (Print:
                                addText(@temp0)
                                addIcon((send gEgo:view) (send gEgo:loop) (send gEgo:cel))
                                init()
                            )
                        )
                        (case KEY_ALT_g
                        	// Set global
                            = temp0 0
                            EditPrint(@temp0 6 "Variable No.")
                            = gOldCastFirst ReadNumber(@temp0)
                            = temp0 0
                            EditPrint(@temp0 6 "Value")
                            = gEgo[gOldCastFirst] ReadNumber(@temp0)
                            = temp0 0
                        )
                        (case KEY_ALT_h
                        	// Show global
                            = temp0 0
                            (Print:
                                addText("Global number:")
                                addEdit(@temp0 6 0 12)
                                init()
                            )
                            = gOldCastFirst ReadNumber(@temp0)
                            (if (IsObject(gEgo[gOldCastFirst]))
                                Format(@temp0 " Global %d: %s " gOldCastFirst (send gEgo[gOldCastFirst]:name))
                            )(else
                                Format(@temp0 " Global %d: %d " gOldCastFirst gEgo[gOldCastFirst])
                            )
                            TextPrint(@temp0)
                        )
                        (case KEY_ALT_i
                        	// Something about inventory
                            (send (ScriptID(16 0)):doit())
                        )
                        (case KEY_ALT_j
                        	// Show cast
                            = gOldCastFirst 0
                            (while (< gOldCastFirst (send gOldCast:size))
                                = temp164 (send gOldCast:at(gOldCastFirst))
                                (if (not & (send temp164:signal) $0004)
                                    Format(@temp0 10 1 (send ((send temp164:{-super-})):name) (send temp164:view) (send temp164:loop) (send temp164:cel) (send temp164:x) (send temp164:y) (send temp164:z) (send temp164:heading) (send temp164:priority) (send temp164:signal) 
                                        (if ((send temp164:isKindOf(Actor)))
                                            (send temp164:illegalBits)
                                        )(else
                                            -1
                                        )
)
                                    (Print:
                                        addText(@temp0 CelWide((send temp164:view) (send temp164:loop) (send temp164:cel)) 0)
                                        window(SysWindow)
                                        addTitle((send temp164:name))
                                        addIcon((send temp164:view) (send temp164:loop) (send temp164:cel) 0 0)
                                        init()
                                    )
                                )
                                ++gOldCastFirst
                            )
                        )
                        (case KEY_ALT_k
                        	// Show palette
                            = temp160 GetPort()
                            SetPort(0)
                            = temp171 5
                            = temp172 16
                            = temp167 15
                            = temp168 80
                            = temp170 (+ temp167 (* 34 temp171))
                            = temp169 (+ temp168 (* 10 temp172))
                            = temp165 Graph(grSAVE_BOX temp167 temp168 temp170 temp169 1)
                            Graph(grFILL_BOX temp167 temp168 temp170 temp169 1 255)
                            = temp166 0
                            (while (< temp166 256)
                                Graph(grFILL_BOX + (+ temp167 temp171) (* temp171 (/ temp166 8)) + (+ temp168 temp172) (* 16 (% temp166 8)) + (+ (+ temp167 temp171) temp171) (* temp171 (/ temp166 8)) + (+ (+ temp168 temp172) temp172) (* temp172 (% temp166 8)) 1 temp166)
                                ++temp166
                            )
                            Graph(grUPDATE_BOX temp167 temp168 temp170 temp169 1)
                            (while (TRUE)
                                = newEvent (Event:new())
                                (if ((== (send newEvent:type) 1) or (== (send newEvent:type) 4))
                                    break
                                )
                                (send newEvent:dispose())
                            )
                            (send newEvent:dispose())
                            Graph(grRESTORE_BOX temp165)
                            Graph(grUPDATE_BOX temp167 temp168 temp170 temp169 1)
                            SetPort(temp160)
                        )
                        (case KEY_ALT_l
                        	// Set flag
                            = temp0 0
                            = gOldCastFirst GetNumber("Flag No.")
                            Bset(gOldCastFirst)
                        )
                        (case KEY_ALT_m
                        	// Clear flag
                            = temp0 0
                            = gOldCastFirst GetNumber("Flag No.")
                            Bclear(gOldCastFirst)
                        )
                        (case KEY_ALT_n
                        	// Show flag
                            = temp0 0
                            = gOldCastFirst GetNumber("Flag No.")
                            (if (Btest(gOldCastFirst))
                                TextPrint("TRUE")
                            )(else
                                TextPrint("FALSE")
                            )
                        )
                        (case KEY_ALT_p
                        	// Priority screen
                            localproc_0052()
                            Show(PRIORITY)
                        )
                        (case KEY_ALT_q
                        	// Detail level
                            (send gGame:detailLevel(1))
                        )
                        (case KEY_ALT_r
                        	// Show room info
                            Format(@temp0 10 3 (send gRoom:name) gModNum (send gRoom:curPic) (send gRoom:style) (send gRoom:horizon) (send gRoom:north) (send gRoom:south) (send gRoom:east) (send gRoom:west) 
                                (if (IsObject((send gRoom:script)))
                                    (send ((send gRoom:script)):name)
                                )(else
                                    "..none.."
                                )
								  )
                            (Print:
                                width(120)
                                addText(@temp0)
                                init()
                            )
                            (send gGame:showMem())
                        )
                        (case KEY_ALT_s
                        	// Show message
                            = temp0 0
                            (if ((Print:
                                addText("Which Format?")
                                addButton(0 "String" 0 12)
                                addButton(1 "Message" 50 12)
                                init()
                            )
)
                                = temp174 GetNumber("Noun?" 0)
                                = temp175 GetNumber("Verb?" 0)
                                = temp176 GetNumber("Case?" 0)
                                = temp177 GetNumber("Sequence?" 0)
                                Message(msgGET temp174 temp175 temp176 temp177 @temp0)
                            )(else
                                EditPrint(@temp0 50 "String to display?")
                            )
                            = temp167 GetNumber("Y Parameter?" 0)
                            = temp168 GetNumber("X Parameter?" 0)
                            = gOldCastFirst GetNumber("Box Width?" 0)
                            = theGFont GetNumber("Font Number?" 0)
                            (if (not theGFont)
                                = theGFont gFont
                            )
                            (Print:
                                posn(temp168 temp167)
                                width(gOldCastFirst)
                                font(theGFont)
                                addText(@temp0)
                                init()
                            )
                        )
                        (case KEY_ALT_t
                        	// Teleport
                            (if (gDialog)
                                (send gDialog:dispose())
                            )
                            (Print:
                                addText("Which room do you want?")
                                addEdit(@temp0 6 115 35)
                                init()
                            )
                            (if (temp0 and (> (= gOldCastFirst ReadNumber(@temp0)) 0))
                                (send gRoom:newRoom(gOldCastFirst))
                            )
                        )
                        (case KEY_ALT_u
                        	// Give hands on
                            (User:
                                canInput(1)
                                canControl(1)
                            )
                            (send gSq5IconBar:enable(0 1 2 3 5 6))
                        )
                        (case $16
                            Show(VISUAL)
                        )
                        (case KEY_ALT_w
                        	// Feature writer
                            (FeatureWriter:doit())
                        )
                        (case KEY_ALT_x
                        	// Quit
                            = gQuitGame 1
                        )
                        (case KEY_ALT_v
                        	// Show version
                            (Print:
                                addText("Version number:" 0 0)
                                addText(gVersion 0 14)
                                init()
                            )
                        )
                        (case KEY_ALT_f
                        	// Feature outlines
                            = temp167 0
                            (while (< temp167 (send gOldCast:size))
                                Graph(grFILL_BOX (send ((send gOldCast:at(temp167))):brTop) (send ((send gOldCast:at(temp167))):brLeft) (send ((send gOldCast:at(temp167))):brBottom) (send ((send gOldCast:at(temp167))):brRight) 1 gColorWindowForeground -1 -1)
                                ++temp167
                            )
                        )
                        (case KEY_ALT_z
                            = gQuitGame 1
                        )
                        (case KEY_QUESTION
                            TextPrint("Debug options:      (Page 1 of 5)\n\n   A - Show cast\n   B - Polygon editor\n   C - Show control map\n   D - Place an actor\n   E - Show ego info\n   F - Show feature outlines\n   G - Set global\n")
                            TextPrint("Debug options:      (Page 2 of 5)\n\n   H - Show global\n   I - Get inventory item\n   J - Justify text on screen\n   K - Show palette\n   L - Set flag\n   M - Clear flag\n   N - Show flag\n")
                            TextPrint("Debug options:      (Page 3 of 5)\n\n   P - Show priority map\n   Q - Set Detail to 1\n   R - Show room info/free memory\n   S - Show a string or message\n   T - Teleport\n   U - Give HandsOn\n")
                            TextPrint("Debug options:      (Page 4 of 5)\n\n   V - Show visual map\n   W - Feature writer\n   Y - View obstacles\n   X,Z - Quick quit\n")
                            TextPrint("Debug options:      (Page 5 of 5)\n\n  A=Alt, C=Ctrl, L=Left shift, R=Right shift\n\n  Left click:\n    A       Move ego\n    CL      Show ego\n    CR      Show room\n    CA      Show position\n")
                        )
                        (default 
                            (send pEvent:claimed(0))
                        )
                    )
                )
                (case evMOUSEBUTTON
                    (switch ((send pEvent:modifiers))
                        (case (| emCTRL emALT)
	                    	// Show mouse pos?
                            (send pEvent:claimed(1))
                            Format(@temp0 10 4 (send pEvent:x) (send pEvent:y))
                            = temp160 (Print:
                                    posn(160 10)
                                    font(999)
                                    modeless(1)
                                    addText(@temp0)
                                    init()
                                )
                            (while (<> evMOUSERELEASE (send ((= newEvent (Event:new()))):type))
                                (send newEvent:dispose())
                            )
                            (send newEvent:dispose())
                            (send temp160:dispose())
                        )
                        (case (| emCTRL emRIGHT_SHIFT)
                            (send pEvent:
                                type(evKEYBOARD)
                                message(KEY_ALT_r)
                            )
                            (self:handleEvent(pEvent))
                        )
                        (case (| emCTRL emLEFT_SHIFT)
                            (send pEvent:
                                type(evKEYBOARD)
                                message(KEY_ALT_e)
                            )
                            (self:handleEvent(pEvent))
                        )
                        (case emALT
                            (send pEvent:claimed(1))
                            = temp178 (send gGame:setCursor(996))
                            = userAlterEgo (User:alterEgo)
                            = gOldCastFirst (send userAlterEgo:signal)
                            (send userAlterEgo:startUpd())
                            (while (<> 2 (send ((= newEvent (Event:new()))):type))
                                (send userAlterEgo:
                                    x((send newEvent:x))
                                    y(- (send newEvent:y) 10)
                                )
                                Animate((send gOldCast:elements) 0)
                                (send newEvent:dispose())
                            )
                            (send newEvent:dispose())
                            (send gGame:setCursor(temp178))
                            (send userAlterEgo:signal(gOldCastFirst))
                        )
                    )
                )
            )
    )

)

(instance drawPoly of Code
	(method (doit thePoly)
		(var i, x1, y1, x2, y2, thePoints, index, indexNext)
		(= thePoints (send thePoly:points))
		(for (= i 0) (< i (send thePoly:size)) (++i)
			(= index (+ thePoints (* i 4)))
			(= x1 Memory(memPEEK index))
			(= y1 Memory(memPEEK (+ index 2)))
			(= indexNext (+ thePoints (* (% (+ i 1) (send thePoly:size)) 4)))
			(= x2 Memory(memPEEK indexNext))
			(= y2 Memory(memPEEK (+ indexNext 2)))
			
			// TODO: Different colors for different types
			Graph(grDRAW_LINE y1 x1 y2 x2 15 -1 -1)
		)
		
	)
)