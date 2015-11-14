/*
	The Obj script contains a number of vital base classes, as well as handy procedures.
*/
(version 2)
(include "sci.sh")
(exports
    0 Sign
    1 Modulo
    2 Min
    3 Max
    4 DoesRectContain
    5 IsOneOf
    6 GetValueAt
    7 Perform
)
(use "Main")
(use "Print")
(script 999)


/*
	:returns: 1 if the number is positive, or -1 if it's negative.
*/
(procedure public (Sign number)
    return 
        (if (< number 0)
            -1
        )(else
            > number 0
        )
)

/*
	Performs a modulo operation, and ensures the result is positive.
	
	:returns: The remainder of the division of dividend by divisor.
*/
(procedure public (Modulo dividend divisor)
    = dividend (- dividend (* divisor (/ dividend divisor)))
    (if (< dividend 0)
        = dividend (+ dividend divisor)
    )
    return dividend
)

/*
	.. function:: Min(param1 [...])
	
	Accepts a series of one or more numbers and returns the smallest.
	
	:returns: The smallest of the numbers.
*/
(procedure public (Min param1 param2)
    (var temp0)
    return 
        (if ((== paramTotal 1) or (< param1 (= temp0 Min(rest param2))))
            param1
        )(else
            temp0
        )
)

/*
	.. function:: Max(param1 [...])
	
	Accepts a series of one or more numbers and returns the largest.
	
	:returns: The largest of the numbers.
*/
(procedure public (Max param1 param2)
    (var temp0)
    return 
        (if ((== paramTotal 1) or (> param1 (= temp0 Max(rest param2))))
            param1
        )(else
            temp0
        )
)

/*
	.. function:: DoesRectContain(left top right bottom x y)
	
	.. function:: DoesRectContain(left top right bottom obj)
	
		:param number left: The left side of the rectangle.
		:param number top: The top of the rectangle.
		:param number right: The right side of the rectangle.
		:param number bottom: The bottom of the rectangle.
		:param number x: An x coordinate.
		:param number y: A y coordinate.
		:param heapPtr obj: An object with x and y properties
	
		:returns: TRUE if the rectangle contains the (x, y) coordinate or the object, FALSE otherwise.
*/
(procedure public (DoesRectContain left top right bottom param5 param6)
    return 
        (if ((<= left 
        (if (< paramTotal 6)
            (send param5:x)
        )(else
            param5
        )
) and (<= 
        (if (< paramTotal 6)
            (send param5:x)
        )(else
            param5
        )
 right))
            (if (<= top 
            (if (< paramTotal 6)
                (send param5:y)
            )(else
                param6
            )
)
                <= 
                (if (< paramTotal 6)
                    (send param5:y)
                )(else
                    param6
                )
 bottom
            )
        )(else
            0
        )
)

/*
	.. function:: IsOneOf(thing [...])
	
	::
	
		(= roomNumber 100)
		(= thisWillBeTRUE IsOneOf(roomNumber 50 100 150 200))
	
	:returns: TRUE if thing is equal to one of the parameters that follows it.
*/
(procedure public (IsOneOf param1 params)
    (var temp0)
    = temp0 0
    (while (< temp0 (- paramTotal 1))
        (if (== param1 params[temp0])
            return 
                (if (param1)
                )(else
                    1
                )
        )
        ++temp0
    )
    return 0
)

/*
	Retrieves a value at a specified offset in a buffer. This might be used for
	retrieving the a point in a buffer containing a sequence of points, for instance.

	:param heapPtr buffer: A block of memory containing 16-bit values. 
	:param number offset: An index into that buffer.
	
	:returns: The item at an offset in a buffer.
*/
(procedure public (GetValueAt buffer offset)
    return Memory(memPEEK (+ buffer (* 2 offset)))
)

/*
.. function:: Perform(object methodSelector [...])
	
	Invokes the method on the object, passing in any parameters.
	
	:param heapPtr object: The object on which to perform the method.
	:param selector methodSelector: The name of a method on the object.
	:param ...: Zero or more parameters to be passed to the method.
*/
(procedure public (Perform object methodSelector params)
    (send object:methodSelector(rest params))
)

/*
	The base class used for all SCI Objects. It provides some basic operations
	like asking if an object supports a particular selector, or if an object is
	subclass of a given class.
*/
(class Obj
    (properties)

	// Returns a clone of itself
    (method (new)
        Clone(self)
    )

    (method (init)
    )


    (method (doit)
        return self
    )


    (method (dispose)
        DisposeClone(self)
    )

	// Copies the object's name into the provided buffer.
    (method (showStr buffer)
        StrCpy(buffer name)
    )

	// Prints the object's name on the screen.
    (method (showSelf)
        (var temp0[200])
        TextPrint((self:showStr(@temp0)))
    )

	/*
	.. function:: perform(object [...])
	
		Calls the doit method on object with itself as the first parameter, and forwarding any additional supplied parameters.
		
		:param heapPtr object: Any object with a doit method.
	*/
    (method (perform object params)
        (send object:doit(self rest params))
    )

	/*
		Determines if this object is a particular class, or inherits from that class.
		
		:param class className: A class name, such as Actor or Rm
		
		:returns: TRUE is this object is of type className, or inherits from className.
	*/
    (method (isKindOf className)
        (var obj_super_)
        (if ( (== {-propDict-} (send className:{-propDict-})) and  (== {-classScript-} (send className:{-classScript-})))
        	return TRUE
		)
		(= obj_super_ (self:{-super-}))
		(if (not obj_super_)
			return FALSE
		)
		(if (IsObject(obj_super_))
			return (send obj_super_:isKindOf(className))
		)
		return FALSE
    )

	/*
		:param class theClass: A class, like MoveTo or Feature.
		:returns: TRUE if this object is an instance of this exact class, otherwise FALSE.
		
		Example usage::
		
			(if (send gThing:isMemberOf(MoveTo))
				// Do something...
			)
	*/
    (method (isMemberOf theClass)
    	(if (== theClass self)
    		return TRUE
		)
		(if ( (& (send theClass:{-info-}) $8000) and (not (& {-info-} $8000)))
			return (== {-propDict-} (send theClass:{-propDict-}))
		)
		return FALSE
    )

	/*
		:param selector selectorName: A property or method selector.
	
		:returns: TRUE if this object has selectorName as a property or method.
		
		Example usage::
		
			(if (send theThing:respondTo(#doit))
				(send theThing:doit())
			)
	*/
    (method (respondsTo selectorName)
        RespondsTo(self selectorName)
    )

	/*
		Returns the object itself. This is a convenient shortcut when you want to call a series of selectors on
		a new object and then pass that object directly to a function::

			// Create a new polygon and add it to the room obstacles.		
			(send gRoom:addObstacle((send ((Polygon:new())):
					type(PBarredAccess)
					points(somePoints)
					size(4)
					dynamic(TRUE)
					yourself()
									)))
	*/
    (method (yourself)
        return self
    )

)

/*
	Code is just a convenient base class to inherit from when
	creating classes that simply have a doit() method that performs some
	functionality.
*/
(class Code of Obj
    (properties)

    (method (doit)
    )
)

// The Collect class is used to handle a collection of elements, such as objects.
(class Collect of Obj
    (properties
        elements 0
        size 0
    )

	/*
	.. function:: doit([...])
	
		Calls the doit method on each element, forwarding any parameters given.
	*/
    (method (doit params)
        (self:eachElementDo(#doit rest params))
    )


    (method (dispose)
        (if (elements)
            (self:eachElementDo(#dispose))
            DisposeList(elements)
        )
        = size (= elements 0)
        (super:dispose())
    )

	// Fills buffer with a text stating the object name and collection size.
    (method (showStr buffer)
        Format(buffer 999 0 name size)
    )

	// For the object and each of its elements, this prints the object's name and collection size on the screen in a dialog box.
    (method (showSelf)
        (var temp0[40])
        TextPrint((self:showStr(@temp0)))
        (self:eachElementDo(#showSelf))
    )

	/*
	.. function:: add([node ...])
	
		Adds the given nodes to the collection of elements. It returns a pointer to itself.
		If no nodes are supplied, it is simply ensured that the list is initialized.
		
		:param node: An object to add to the collection.
		:returns: Itself.
	*/
    (method (add nodes)
        (var temp0, temp1, temp2)
        (if (not elements)
            = elements NewList()
        )
        = temp1 0
        (while (< temp1 paramTotal)
            (if (not (self:isDuplicate(nodes[temp1])))
                AddToEnd(elements NewNode(nodes[temp1] nodes[temp1]))
                ++size
            )
            ++temp1
        )
        return self
    )

	/*
	.. function:: delete([node ...])
	
		Deletes the specified nodes from the collection of elements. It returns a pointer to the object.
		
		:param node: An object to delete from the collection.
		:returns: Itself.
	*/
    (method (delete nodes)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            (if (DeleteKey(elements nodes[temp0]))
                --size
            )
            ++temp0
        )
        return self
    )

	/*
	.. function:: eachElementDo(aSelector [...])
	
		For each element, this calls aSelector (a method or property), forwarding any other given parameters to it.
		
		:param selector aSelector: A selector (e.g. #doit, or #moveSpeed).
		
		Example usage::
		
			(myPolyList:eachElementDo(#perform drawPoly))
	*/
    (method (eachElementDo aSelector params)
        (var temp0, temp1, temp2)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if (not IsObject(temp2))
                return 
            )
            (send temp2:aSelector(rest params))
            = temp0 temp1
        )
    )

	/*
	.. function:: firstTrue(aSelector [...])	
	
		For each element, this calls aSelector (a method or property), and if given, the rest of the parameters.
		The first send to an element to return TRUE will cause firstTrue to return a pointer to that object.
		If no send returns TRUE, firstTrue will return NULL.
		
		:param selector aSelector: A selector (e.g. #doit, or #moveSpeed).
		:returns: The first object that returned TRUE, or NULL if no objects returned TRUE.
		
	*/
    (method (firstTrue aSelector params)
        (var temp0, temp1, temp2)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if ((send temp2:aSelector(rest params)))
                return temp2
            )
            = temp0 temp1
        )
        return 0
    )

	/*
	.. function:: allTrue(aSelector [...])	
	
		For each element, this calls aSelector (a method or property), and if given, the rest of the parameters.
		If any send to an element returns FALSE, allTrue will return FALSE as well. Otherwise, if all sends have returned TRUE, so will allTrue.	
		
		:param selector aSelector: A selector (e.g. #doit, or #moveSpeed).
		:returns: TRUE if all objects returned TRUE, otherwise FALSE.
	*/
    (method (allTrue aSelector params)
        (var temp0, temp1, temp2)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if (not (send temp2:aSelector(rest params)))
                return FALSE
            )
            = temp0 temp1
        )
        return TRUE
    )

	// :returns: TRUE if the object contains node in its collection of elements.
    (method (contains node)
        FindKey(elements node)
    )

	// :returns: TRUE if the collection contains no nodes (elements). Otherwise, it returns FALSE.
    (method (isEmpty)
        (if (== elements 0)
        )(else
            EmptyList(elements)
        )
    )

	// :returns: a pointer to the first node in the collection.
    (method (first)
        FirstNode(elements)
    )

	// :returns: a pointer to the node after the specified one.
    (method (next node)
        NextNode(node)
    )

	// Deletes all the nodes (elements) from the collection.
    (method (release)
        (var temp0, temp1)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            (self:delete(NodeValue(temp0)))
            = temp0 temp1
        )
    )


    (method (isDuplicate)
        return 0
    )

)

// The List class is an extension of the Collect class. It is used to handle a list of elements such as objects that can be accessed by index.
(class List of Collect
    (properties
        elements 0
        size 0
    )

    (method (showStr buffer)
        Format(buffer 999 1 name size)
    )

	// Returns a pointer to the node at the specified index.
    (method (at theIndex)
        (var temp0)
        = temp0 FirstNode(elements)
        (while (theIndex and temp0)
            --theIndex
            = temp0 NextNode(temp0)
        )
        return 
            (if (temp0)
                NodeValue(temp0)
            )(else
                0
            )
    )

	// Returns a pointer to the last node in the collection.
    (method (last)
        LastNode(elements)
    )

	// Returns a pointer to the node before the specified one in the collection.
    (method (prev node)
        PrevNode(node)
    )

	/*
	.. function:: addToFront([nodes ...])

		Adds the specified nodes to the front of the element list. It returns a pointer to the itself.
	*/
    (method (addToFront nodes)
        (var temp0)
        (if (not elements)
            = elements NewList()
        )
        = temp0 (- paramTotal 1)
        (while (<= 0 temp0)
            (if (not (self:isDuplicate(nodes[temp0])))
                AddToFront(elements NewNode(nodes[temp0] nodes[temp0]))
                ++size
            )
            --temp0
        )
        return self
    )

	/*
	.. function:: addToEnd([nodes ...])

		Adds the specified nodes to the end of the element list. It returns a pointer to the itself.
	*/
    (method (addToEnd nodes)
        (var temp0)
        (if (not elements)
            = elements NewList()
        )
        = temp0 0
        (while (< temp0 paramTotal)
            (if (not (self:isDuplicate(nodes[temp0])))
                AddToEnd(elements NewNode(nodes[temp0] nodes[temp0]))
                ++size
            )
            ++temp0
        )
        return self
    )

	/*
	.. function:: addAfter(node [nodes ...])

		Adds the specified nodes to the element list after node. It returns a pointer to itself.
	*/
    (method (addAfter node nodes)
        (var temp0, temp1, temp2)
        = temp2 FindKey(elements node)
        (if (temp2)
            --paramTotal
            = temp0 0
            (while (< temp0 paramTotal)
                (if (not (self:isDuplicate(nodes[temp0])))
                    = temp2 AddAfter(elements temp2 NewNode(nodes[temp0] nodes[temp0]))
                    ++size
                )
                ++temp0
            )
        )
        return self
    )

	// Returns the index of pNode in the list of elements.
    (method (indexOf node)
        (var temp0, temp1)
        = temp0 0
        = temp1 FirstNode(elements)
        (while (temp1)
            (if (== node NodeValue(temp1))
                return temp0
            )
            ++temp0
            = temp1 NextNode(temp1)
        )
        return -1
    )
)

// The Set class extends the List class by ensuring there can be no duplicate objects in the set.
(class Set of List
    (properties
        elements 0
        size 0
    )

	// Copies the object's name into the provided buffer.
    (method (showStr buffer)
        Format(buffer 999 2 name size)
    )

    (method (isDuplicate node)
        (self:contains(node))
    )
)

// The EventHandler class is an extension of the Set class. Is used as a multi event handler.
(class EventHandler of Set
    (properties
        elements 0
        size 0
    )

	// For each element in the list, it calls handleEvent with pEvent as the event object. It then returns whether or not the pEvent has been claimed.
    (method (handleEvent pEvent)
        (var temp0, temp1, temp2, temp3, temp4)
        = temp3 Clone(pEvent)
        = temp0 FirstNode(elements)
        (while (temp0 and not (send temp3:claimed))
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if (not IsObject(temp2))
                break
            )
            (send temp2:handleEvent(temp3))
            = temp0 temp1
        )
        = temp4 (send temp3:claimed)
        (send temp3:dispose())
        return temp4
    )
)

/*
	The Script class (different than a script file) is a very important part of the game. A Script instance can be attached
	to most game objects, including Props, Actors, and room objects. They allow you to attach pluggable behavior to an object.
	In fact, Script instances can themselves have Scripts attached to them.
*/
(class Script of Obj
    (properties
        client 0
        state $ffff
        start 0
        timer 0
        cycles 0
        seconds 0
        lastSeconds 0
        ticks 0
        lastTicks 0
        register 0
        script 0
        caller 0
        next 0
    )

	/*
		Sets the client property to theClient. If the other parameters are specified, it sets the caller property to theCaller,
		and the register property to theRegister. Finally, it calls the changeState method with the start property as its parameter.
	*/
    (method (init theClient theCaller theRegister)
        = lastTicks gLastTicks
        (if (>= paramTotal 1)
            (send (= client theClient):script(self))
            (if (>= paramTotal 2)
                = caller theCaller
                (if (>= paramTotal 3)
                    = register theRegister
                )
            )
        )
        = state (- start 1)
        (self:cue())
    )

	/*
		If the script property is not NULL, it calls the doit method of it.
		Next, if cycles property is not zero, it decrements it. If after decrementing, it is equal to zero, it calls the cue() method.
		
		If the cycles property was zero to begin with, and the seconds property is not equal to zero, it compares the actual seconds to the lastSeconds property.
		If they are not equal, it sets lastSeconds to the current seconds (from GetTime(gtTIME_OF_DAY)) and decrements the seconds property. If the seconds property is equal to zero, it calss the cue() method.	
	*/
    (method (doit)
        (var theLastSeconds)
        (if (script)
            (send script:doit())
        )
        (if (cycles)
            (if (not --cycles)
                (self:cue())
            )
        )(else
            (if (seconds)
                = theLastSeconds GetTime(gtTIME_OF_DAY)
                (if (<> lastSeconds theLastSeconds)
                    = lastSeconds theLastSeconds
                    (if (not --seconds)
                        (self:cue())
                    )
                )
            )(else
                (if (ticks and (<= (= ticks (- ticks Abs((- gLastTicks lastTicks)))) 0))
                    = ticks 0
                    (self:cue())
                )
            )
        )
        = lastTicks gLastTicks
    )


    (method (dispose)
        (var temp0)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (IsObject(timer))
            (send timer:dispose())
        )
        (if (IsObject(client))
            = temp0 
                (if (IsObject(next))
                    next
                )(else
                    (if (next)
                        ScriptID(next)
                    )
                )
            (send client:script(temp0))
            (if (not temp0)
                0
            )(else
                (if (== gRoomNumber gModNum)
                    (send temp0:init(client))
                )(else
                    (send temp0:dispose())
                )
            )
        )
        (if (IsObject(caller) and (== gRoomNumber gModNum))
            (send caller:cue(register))
        )
        = script (= timer (= client (= next (= caller 0))))
        (super:dispose())
    )

	/*
		Sets the state property to the newState. This method is commonly overridden by
		Script instances to implement custom behavior.
	*/
    (method (changeState newState)
        = state newState
    )

	// Switches to the next state, forwarding any parameters to changeState.
    (method (cue params)
        (if (client)
            (self:changeState((+ state 1) rest params))
        )
    )

	/*
		If the script property is not null, it calls the script's handleEvent() method with pEvent has its parameter.
		It then returns whether of not the pEvent object has been claimed.
	*/
    (method (handleEvent pEvent)
        (if (script)
            (send script:handleEvent(pEvent))
        )
        (send pEvent:claimed)
    )

	// Attaches a script to this Script object itself and initializes it with any sendParams provided.
    (method (setScript theScript sendParams)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (theScript)
            (send theScript:init(self rest sendParams))
        )
    )
)

/*
	The Event class is one of the most widely used classes in SCI games. It is used to find out if the user has given input.
	
	Event types:
	
	================= =========================================
	Event type          Description
	================= =========================================
	evNULL              A null event
	evMOUSEBUTTON       A mouse button press event
	evMOUSERELEASE      A mouse button press event
	evKEYBOARD          A keyboard event
	evJOYSTICK          A joystick event
	evMOUSEKEYBOARD     A mouse button press or keyboard event
	evALL_EVENTS        Check for all events	
	================= =========================================

	The following event types also exist, but are not generated by the Event class itself.
	These events are *manufactured* by the game code instead of directly caused by the user.
	
	- evVERB								
	- evMOVE
	- evHELP
	- evHELPVERB
	
	Event message:
	
	The event **message** property is used mainly for keyboard events, where it contains the keycode.
	
	The event **modifiers** property can contain any of the following flags for keyboard events:
	
	- emRIGHT_SHIFT
	- emLEFT_SHIFT
	- emSHIFT
	- emCTRL
	- emALT
	- emSCR_LOCK
	- emNUM_LOCK
	- emCAPS_LOCK
	- emINSERT
	
	For mouse events, the following flags may be set in **modifiers**:
	
	- emLEFT_BUTTON
	- emRIGHT_BUTTON
*/
(class Event of Obj
    (properties
        type $0000			// evKEYBOARD, evMOUSEBUTTON, etc...
        message 0			// The key or mouse button that was pressed.
        modifiers $0000		// emSHIFT, emCTRL or any of the other em\* defines.
        y 0					// The x for mouse events.
        x 0					// The y for mouse events.
        claimed FALSE		// Has the event been claimed?
        port 0
    )

	/*
	.. function:: new([eventType])
	
		Creates a new instance of the Event class. It then fills its properties accordingly with the lastest event information information.
		If eventType is specified, it retreives the event(s) specified by it. Otherwise, it retrieves all events. Finally, it returns a pointer to itself.
	*/
    (method (new eventType)
        (var newSuper)
        = newSuper (super:new())
        GetEvent(
            (if (paramTotal)
                eventType
            )(else
                evALL_EVENTS
            )
 			newSuper)
 
        return newSuper
    )

	// Brings the event into the current port's local coordinates.
    (method (localize)
        (var thePort)
        (if (not & type evVERB)
            = thePort GetPort()
            (if (not port)
                GlobalToLocal(self)
            )(else
                (if (<> port thePort)
                    SetPort(port)
                    LocalToGlobal(self)
                    SetPort(thePort)
                    GlobalToLocal(self)
                )
            )
            = port thePort
        )
        return self
    )


    (method (globalize)
        (var temp0)
        (if (not & type evVERB)
            = temp0 GetPort()
            (if (== port temp0)
                LocalToGlobal(self)
            )(else
                (if (port)
                    SetPort(port)
                    LocalToGlobal(self)
                    SetPort(temp0)
                )
            )
            = port 0
        )
        return self
    )
)

// The Cursor class represents an onscreen mouse cursor.
(class Cursor of Obj
    (properties
        view 0
        loop 0
        cel 0
        x 0
        y 0
        hotSpotX 0
        hotSpotY 0
        hidden 0
    )

	// Initializes the cursor and sets it as the active cursor.
    (method (init)
        (if (hotSpotX or hotSpotY)
            SetCursor(view loop cel hotSpotX hotSpotY)
        )(else
            SetCursor(view loop cel)
        )
    )

	// Sets the position of the cursor.
    (method (posn x y)
        SetCursor(x y)
    )


	// Sets the hotspot of the cursor.
    (method (posnHotSpot theHotSpotX theHotSpotY)
        = hotSpotX theHotSpotX
        = hotSpotY theHotSpotY
        (self:init())
    )

	// Changes the view loop of the cursor.
    (method (setLoop theLoop)
        = loop theLoop
        (self:init())
    )

	// Changes the cel of the cursor.
    (method (setCel theCel)
        = cel theCel
        (self:init())
    )

	// Shows or hides the cursor.
    (method (showCursor show)
        (if (paramTotal)
            SetCursor(show)
        )
    )

)
