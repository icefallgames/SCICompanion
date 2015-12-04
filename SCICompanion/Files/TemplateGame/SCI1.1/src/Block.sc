(version 2)
(include "sci.sh")
(use "System")
(script 949)


/*
	This class defines a rectangular area that an Actor can't enter. In SCI1.1, this has mostly been
	superceded by Polygons.
	
	Example usage::
	
		(theFrog:observeBlocks((send ((Blk:new())):
			left(150)
			top(140)
			right(170)
			bottom(160)
			yourself()
		)
*/
(class Block of Object
    (properties
    	name "Blk"
        top 0
        left 0
        bottom 0
        right 0
    )

    (method (doit param1)
        return 
            (if (((<= (send param1:brBottom) top) or (> (send param1:brTop) bottom)) or (< (send param1:brRight) left))
            )(else
                >= (send param1:brLeft) right
            )
    )
)

/*
	This class defines a rectangular area to which an Actor is constrained. In SCI1.1, this has mostly been
	superceded by Polygons.
	
	Example usage::
	
		(theFrog:observeBlocks((send ((Blk:new())):
			left(150)
			top(140)
			right(170)
			bottom(160)
			yourself()
		)
*/
(class Cage of Block
    (properties
        top 0
        left 0
        bottom 0
        right 0
    )

    (method (doit param1)
        return 
            (if (((>= (send param1:brTop) top) and (>= (send param1:brLeft) left)) and (<= (send param1:brBottom) bottom))
                <= (send param1:brRight) right
            )(else
                0
            )
    )

)
