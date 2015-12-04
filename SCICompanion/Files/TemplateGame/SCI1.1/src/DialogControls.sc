// This script contains the following controls that can be used in dialogs: DIcon, DButton, DEdit, DSelector.
(version 2)
(include "sci.sh")
(use "Main")
(use "Controls")
(use "System")
(script 922)


// An icon control.
(class DIcon of Control
    (properties
        type ctlICON
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0			// The keyboard key associated with this control.
        said 0
        value 0			// Arbitrary value associated with this control.
        view 0			// The icon view
        loop 0			// The icon loop
        cel 0			// The icon cel
    )

    (method (setSize)
        = nsRight (+ nsLeft CelWide(view loop cel))
        = nsBottom (+ nsTop CelHigh(view loop cel))
    )

)

// A button control.
(class DButton of Control
    (properties
        type ctlBUTTON
        state $0003
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0			// The keyboard key associated with this control.
        said 0
        value 0			// Arbitrary value associated with this control.
        text 0			// The button text.
        font 0			// The button font.
    )

    (method (dispose param1)
        (if (text and (not paramTotal or not param1))
            Memory(memFREE (self:text))
        )
        (super:dispose())
    )


    (method (setSize)
        (var temp0[4])
        TextSize(@temp0[0] text font 0 0)
        = temp0[2] (+ temp0[2] 2)
        = temp0[3] (+ temp0[3] 2)
        = nsBottom (+ nsTop temp0[2])
        = temp0[3] (* (/ (+ temp0[3] 15) 16) 16)
        = nsRight (+ temp0[3] nsLeft)
    )

)

// An edit control.
(class DEdit of Control
    (properties
        type ctlEDIT
        state $0001
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0			// The keyboard key associated with this control.
        said 0
        value 0			// Arbitrary value associated with this control.
        text 0
        font 0			// The edit control font.
        max 0			// The maximum number of characters allowed.
        cursor 0
    )

    (method (track param1)
        EditControl(self param1)
        return self
    )


    (method (setSize)
        (var temp0[4])
        = font gInputFont
        TextSize(@temp0[0] "M" font 0 0)
        = nsBottom (+ nsTop temp0[2])
        = nsRight (+ nsLeft (/ (* (* temp0[3] max) 3) 4))
        = cursor StrLen(text)
    )
)

// A selector control that lets the player choose from a list of text strings.
(class DSelector of Control
    (properties
        type ctlSELECTOR
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0			// The keyboard key associated with this control.
        said 0
        value 0			// Arbitrary value associated with this control.
        font 0
        x 20
        y 6
        text 0
        cursor 0
        topString 0
        mark 0
    )

    (method (handleEvent pEvent)
        (var temp0, temp1[3], temp4, temp5[4])
        (if ((send pEvent:claimed))
            return 0
        )
        = temp0 0
        (switch ((send pEvent:type))
            (case evKEYBOARD
                (send pEvent:claimed(
                    (switch ((send pEvent:message))
                        (case KEY_NUMPAD7
                            (self:retreat(50))
                        )
                        (case KEY_NUMPAD1
                            (self:advance(50))
                        )
                        (case KEY_PAGEUP
                            (self:advance((- y 1)))
                        )
                        (case KEY_PAGEDOWN
                            (self:retreat((- y 1)))
                        )
                        (case KEY_NUMPAD2
                            (self:advance(1))
                        )
                        (case KEY_UP
                            (self:retreat(1))
                        )
                        (default 
                            FALSE
                        )
                    )
))
            )
            (case evMOUSEBUTTON
                (if ((self:check(pEvent)))
                    (send pEvent:claimed(TRUE))
                    (if (< (send pEvent:y) (+ nsTop 10))
                        (do 
                            (self:retreat(1))
                        ) while (not not MouseStillDown())
                    )(else
                        (if (> (send pEvent:y) (- nsBottom 10))
                            (do 
                                (self:advance(1))
                            ) while (not not MouseStillDown())
                        )(else
                            TextSize(@temp5[0] "M" font 0 0)
                            = temp4 (/ (- (send pEvent:y) (+ nsTop 10)) temp5[2])
                            (if (> temp4 mark)
                                (self:advance((- temp4 mark)))
                            )(else
                                (self:retreat((- mark temp4)))
                            )
                        )
                    )
                )
            )
        )
        return 
            (if ((send pEvent:claimed) and (& state csEXIT))
                self
            )(else
                0
            )
    )


    (method (setSize)
        (var temp0[4])
        TextSize(@temp0[0] "M" font 0 0)
        = nsBottom (+ (+ nsTop 20) (* temp0[2] y))
        = nsRight (+ nsLeft (/ (* (* temp0[3] x) 3) 4))
        = topString (= cursor text)
        = mark 0
    )

	/*
		Returns the index of specified string in the list of items. If the string is not an item in the list, it returns -1.	
		
		:param string findText: The text to search for.
	*/
    (method (indexOf findText)
        (var theText, temp1)
        = theText text
        = temp1 0
        return (while (< temp1 300)
                (if (== 0 StrLen(theText))
                    return -1
                )
 
                (if (not StrCmp(findText theText))
                    return temp1
                )
 = theText (+ theText x) ++temp1  )
    )

	/*
		Returns the text at the specified position in the list.
		
		:param number index: The index into the list.
		:returns: A pointer to the text.
	*/
    (method (at index)
        return + text (* x index)
    )

	// Increments the selected item's index by amount.
    (method (advance amount)
        (var temp0)
        (if (not StrAt(cursor 0))
            return not StrAt(cursor 0)
        )
        = temp0 0
        (while (amount and StrAt(cursor x))
            = temp0 1
            = cursor (+ cursor x)
            (if (< (+ mark 1) y)
                ++mark
            )(else
                = topString (+ topString x)
            )
            --amount
        )
        return 
            (if (temp0)
                (self:draw())
                1
            )(else
                0
            )
    )

	// Decrements the selected item's index by amount.
    (method (retreat amount)
        (var temp0)
        = temp0 0
        (while (amount and (<> cursor text))
            = temp0 1
            = cursor (- cursor x)
            (if (mark)
                --mark
            )(else
                = topString (- topString x)
            )
            --amount
        )
        return 
            (if (temp0)
                (self:draw())
                1
            )(else
                0
            )
    )
)

// A list of controls.
(class Controls of List
    (properties
        elements 0
        size 0
    )

	// Updates the size of each control and tells it to draw.
    (method (draw)
        (self:eachElementDo(#setSize))
        (self:eachElementDo(#draw))
    )

	// Directs the event to the first focused control.
    (method (handleEvent pEvent)
        (var temp0)
        (if ((send pEvent:claimed))
            return 0
        )
        = temp0 (self:firstTrue(#handleEvent pEvent))
        (if (temp0 and not (send ((= temp0 (self:firstTrue(#handleEvent pEvent)))):checkState(csEXIT)))
            (send temp0:doit())
            = temp0 0
        )
        return temp0
    )
)
