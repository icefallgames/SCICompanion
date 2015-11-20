/*
	This is the script that you modify to add additional inventory item instances that inherit from :class:`InventoryItem`.
	
	An example might be::
	
		(instance Hammer of InventoryItem
			(properties
				view 900
				loop 1
				cursor 900			// Optional cursor when using this item.
				message V_HAMMER	// Optional verb associated with the item.
				signal $0002
				noun N_HAMMER		// noun from message resource 0
			)
		)
		
	Then in templateInventory::init(), add the inventory item to the add() call.
	
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(include "15.shm")
(exports
    0 invCode
    1 invWin
)
(use "Main")
(use "ScrollableInventory")
(use "ScrollInsetWindow")
(use "Print")
(use "IconItem")
(use "InventoryItem")
(use "Object")
(script INVENTORY_SCRIPT)

(instance templateInventory of ScrollableInventory
    (properties)

    (method (init param1)
        (super:init(rest param1))
        (invWin:
            color(gColorWindowForeground)
            back(3)
            topBordColor(5)
            lftBordColor(4)
            rgtBordColor(2)
            botBordColor(1)
            insideColor(2)
            topBordColor2(1)
            lftBordColor2(1)
            botBordColor2(6)
            rgtBordColor2(6)
        )
        (self:
        	// Add inventory items here.
            // add(Money AThing AnotherThing)
            eachElementDo(#lowlightColor 2)
            add(invLook invSelect invHelp invUp invDown ok)
        )
        (self:
            state(2048)
            upIcon(invUp)
            downIcon(invDown)
            window(invWin)
            helpIconItem(invHelp)
            selectIcon(invSelect)
            okButton(ok)
            numCols(5)
            scrollAmount(5)
            dispAmount(10)
            empty(13)
            normalHeading(15)
            eachElementDo(#highlightColor 0)
            eachElementDo(#modNum INVENTORY_SCRIPT)
            eachElementDo(#init)
        )
    )

)
(instance public invCode of Code
    (properties)

    (method (init)
        = gInv templateInventory
        (send gInv:init())
    )

)
(instance public invWin of ScrollInsetWindow
    (properties
        priority -1
        topBordHgt 30
        botBordHgt 5
    )

    (method (open param1)
        (invLook:nsLeft((- (/ (- (self:right) (self:left)) 2) 84)))
        (invLook:nsTop(2))
        (super:open(rest param1))
    )

)
(instance invUp of IconItem
    (properties
        view 991
        loop 5
        cel 0
        cursor 999
        maskView 991
        maskLoop 5
        maskCel 2
        lowlightColor 5
        noun N_UPICON
        helpVerb V_HELP
    )

    (method (select params)
        (if ((super:select(rest params)))
            (send gInv:scroll(-1))
        )
        return 0
    )

)
(instance invDown of IconItem
    (properties
        view 991
        loop 6
        cel 0
        cursor 999
        maskView 991
        maskLoop 6
        maskCel 2
        lowlightColor 5
        noun N_DOWNICON
        helpVerb V_HELP
    )

    (method (select params)
        (if ((super:select(rest params)))
            (send gInv:scroll(1))
        )
        return 0
    )

)
(instance ok of IconItem
    (properties
        view 991
        loop 3
        cel 0
        cursor 999
        signal $0043
        lowlightColor 5
        noun N_CLOSEBUTTON
        helpVerb V_HELP
    )
)
(instance invLook of IconItem
    (properties
        view 991
        loop 2
        cel 0
        cursor 981
        message V_LOOK
        signal $0081
        lowlightColor 5
        noun N_DESCICON
        helpVerb V_HELP
    )
)
(instance invHand of IconItem
    (properties
        view 991
        loop 0
        cel 0
        cursor 982
        message V_DO
        lowlightColor 5
        noun N_DOICON
        helpVerb V_HELP
    )
)
(instance invHelp of IconItem
    (properties
        view 991
        loop 1
        cel 0
        cursor 989
        message V_HELP
        lowlightColor 5
        noun N_HELPICON
        helpVerb V_HELP
    )
)
(instance invSelect of IconItem
    (properties
        view 991
        loop 4
        cel 0
        cursor 999
        lowlightColor 5
        noun N_SELECTICON
        helpVerb V_HELP
    )
)

// Declare inventory items here.
/*
(instance Money of InventoryItem
    (properties
        view 900
        loop 1
        cursor 900
        message V_PAY
        signal $0002
        noun N_MONEY
    )
)*/