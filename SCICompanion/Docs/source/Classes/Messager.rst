.. Messager

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Messager (of :class:`Obj`)
==========================

.. class:: Messager

	Defined in Messager.sc.

	
	Messager is responsible for automatically showing sequences of messages,
	including invoking the appropriate :class:`Talker` instances to display the messages.
	
	In general, only one instance of Messager exists in the game, gTestMessager in the main.sc script.
	
	Messager is mostly commonly triggered automatically by the player performing an action on an object (e.g. a verb on a noun).
	However, sometimes you will want to invoke it directly if you have more complex logic to deal with certain situations.
	The principal method of interest for callers is say().
	
	Example usage::
	
		(send gTestMessager:say(noun verb condition seqeuence caller roomNumber))	
	


Properties
==========

Defined in Messager:

=============== ===========
Property        Description
=============== ===========
caller                     
disposeWhenDone            
oneOnly                    
killed                     
oldIconBarState            
curSequence                
lastSequence               
talker                     
=============== ===========


Methods
==========

.. function:: dispose()
	:noindex:



.. function:: cue(param1)
	:noindex:

	Cues the Messager, which triggers the next message in a sequence to be displayed.



.. function:: say(noun [verb cond seq caller modnum])
	:noindex:

.. function:: say(noun [verb cond seq lastSeq caller modnum])
	:noindex:

	Causes the specified message to be shown using its associated :class:`Narrator` or :class:`Talker`. If seq is 0,
	then all messages in the sequence are shown. Otherwise, only the one with the specified sequence is shown.

	:param number noun: The noun of the message to show.
	:param number verb: The verb of the message to show.
	:param number cond: The condition of the message to show.
	:param number seq: The sequence number of the message to show, or 0 to show all messages in a sequence.
	:param number lastSeq: The last sequence to show.
	:param object caller: Optional caller.
	:param number modNum: Optional room number of the message to show (if not specified, the current room is used).

.. function:: say(-1 [caller])
	:noindex:

	This version of say does the same thing as the parameterless sayNext(), but lets you specify a new caller.




.. function:: sayFormat(talkerNumber formatString theCaller params)
	:noindex:

	Directly invokes the :class:`Narrator` or :class:`Talker` with the formatted message string.



.. function:: sayNext()
	:noindex:

.. function:: sayNext(theModNum noun verb cond seq)
	:noindex:

	This displays the next message in a sequence. It should generally only be called by the framework.




.. function:: findTalker(talkerNumber)
	:noindex:

	This message should be overridden in subclasses to return a talker object based upon a talker number.



