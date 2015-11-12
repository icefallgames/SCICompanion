.. Inline assembly

.. include:: /includes/standard.rst

=================
 Inline assembly
=================

To give you complete control over scripting your game, the ability to to inline assembly was implemented. With inline assembly, you can script SCI byte code within the highlevel code.
You can refer to symbols such as class or instance names, or variables.

Syntax::

	(asm
		<asm code>
	)

Example::

	(procedure (SomeProc) 
		(asm 
			 pushi #init
			 push0
			 lofsa {fWindow}
			 send 4
		)
	)	

		(method (doit param1)
			(asm
				pushi   #isBlocked
				pushi   0
				lag     gEgo
				send    4
				bnt     code_3273
				ldi     1
				aTop    completed
				pushi   #motionCue
				pushi   0
				self    4
				jmp     code_327b
	code_3273:  pushi   #doit
				pushi   0
				&rest   param1
				super   PolyPath, 4
	code_327b:  ret     
			)
		)