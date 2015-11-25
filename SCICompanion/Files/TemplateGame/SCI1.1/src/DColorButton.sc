(version 2)
(include "sci.sh")
(use "Controls")
(use "DialogControls")
(use "Object")
(script 32)


/*
	A Button control that lets you set various colors for different states.
*/
(class DColorButton of DButton
    (properties
        type $0001
        state $0003
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0			// Unused
        value 0
        text 0
        font 0
        mode alLEFT		// alLEFT, alCENTER or alRIGHT.
        nfc 0			// Normal foreground color.
        nbc 7			// Normal background color.
        hfc 15			// Highlight foreground color.
        hbc 7			// Highlight background color.
        sfc 7			// Selected foreground color.
        sbc 7			// Selected background color.
    )

    (method (track param1)
        (var temp0, temp1, theSfc, theSbc)
        return 
            (if (== 1 (send param1:type))
                = temp1 0
                (do 
                    = param1 (Event:new(evPEEK))
                    (send param1:localize())
                    = temp0 (self:check(param1))
                    (if (<> temp0 temp1)
                        (if (& state csSELECTED)
                            = theSfc sfc
                            = theSbc sbc
                        )(else
                            = theSfc hfc
                            = theSbc hbc
                        )
                        Display(text dsCOORD nsLeft nsTop dsWIDTH (- nsRight nsLeft) dsFONT font dsCOLOR theSfc dsBACKGROUND theSbc dsALIGN mode)
                        = temp1 temp0
                    )
                    (send param1:dispose())
                ) while (not not MouseStillDown())
                (if (temp0)
                    (if (& state csSELECTED)
                        = theSfc sfc
                        = theSbc sbc
                    )(else
                        = theSfc hfc
                        = theSbc hbc
                    )
                    Display(text dsCOORD nsLeft nsTop dsWIDTH (- nsRight nsLeft) dsFONT font dsCOLOR theSfc dsBACKGROUND theSbc dsALIGN mode)
                )
                return temp0
            )(else
                return self
            )
    )


    (method (setSize param1)
        (var temp0[4])
        TextSize(@temp0[0] text font 
		            (if (paramTotal)
		                param1
		            )(else
		                0
		            )
				)
        = temp0[2] (+ temp0[2] 2)
        = temp0[3] (+ temp0[3] 2)
        = nsBottom (+ nsTop temp0[2])
        = temp0[3] (* (/ (+ temp0[3] 15) 16) 16)
        = nsRight (+ temp0[3] nsLeft)
    )


    (method (draw)
        (var theHfc, theHbc)
        (if (& state csSELECTED)
            = theHfc hfc
            = theHbc hbc
        )(else
            = theHfc nfc
            = theHbc nbc
        )
        Display(text dsCOORD nsLeft nsTop dsWIDTH (- nsRight nsLeft) dsFONT font dsCOLOR theHfc dsBACKGROUND theHbc dsALIGN mode)
    )

)
