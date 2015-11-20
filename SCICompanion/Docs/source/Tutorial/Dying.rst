.. Dying

.. default - domain::js

.. include:: /includes/standard.rst

.. IMPORTANT:: SCI1.1 only.

====================================
Dying
====================================

Having an evil ego walking around is a good time to worry about dying. 

Let's make the ego die when the evil ego gets close. Add a script to the evilEgo. We can delete the changeState method (we don't need it)
and just use doit(). Furthermore, we can override init() in the evilEgo and set the script there. That looks like::

    (instance evilEgo of Actor
        (properties
            view 1
            x 231
            y 131
            signal ignAct
            noun N_EVILEGO
            moveSpeed 30
            cycleSpeed 30
        )
    
        (method (init params)
            (super:init(rest params))
            (self:setScript(evilEgoScript))
        )
    )

    (instance evilEgoScript of Script
        (properties)

        (method (doit)
            (super:doit())
        )
    )


Now in our doit() method we need to see how close the evilEgo is to the player, and kill them if evilEgo is too close:

.. code-block:: python
    :emphasize-lines: 6-8

    (instance evilEgoScript of Script
        (properties)

        (method (doit)
            (super:doit())
            (if (<= (send client:distanceTo(gEgo)) 25)
                Die()
            )
        )
    )

Is it that easy? Yes it is! You can also pass a number to Die to indicate a death reason. This lets you customize your death screen.
The default one in the template game is a little spartan, but you can open DeathRoom.sc and customize to your heart's content when
you're more familiar with SCI.

What if there were a way to fight back?