
;-----------------;
;--- Variables ---;
;-----------------;		

		cblock	    0x20
sel_motor
sig_state
pos1
pos2
pos3
pos4
motor_aux
sel_pin
		endc
		
;--------------------------;
;--- Subrutinas motores ---;
;--------------------------;

;--- Inicialización ---;
		
init_motors	movlw	b'00110001'	;Habilitamos timer 1 con prescaler 1:8
		movwf	t1con
		movlw	b'00001010'	;Compare mode, generates software interrupt
		movwf	ccp1con
		
		movlw	0x02		;Comparamos a mitad de cuenta
		movwf	ccpr1h
		movlw	0xFF
		movwf	ccpr1l
		
		clrf	sig_state
		clrf	sel_motor
		
		; Habilitar interrupciones, modulo CCP1
		bank1
		bsf	pie1,ccp1ie		;habilitamos la interrupcion del modulo
		bank0
		bsf	intcon,peie		;habilitamos interrupciones de perifericos
		
		return
		
;--- CCP1 ISR ---;
		
ISR_CCP1	bank0
		bcf	    pir1,ccp1if		;Borramos la bandera
		clrf	    tmr1h		;ponemos a 0 el timer1
		clrf	    tmr1l
		
		; Do your stuff here
		
		movfw	    sel_motor
		andlw	    b'00000011'		;Mascara para seleccion
		addlw	    0x22
		movwf	    fsr			;Seleccion direccionamiento indirecto
		
		btfsc	    sig_state,0
		goto	    low_state
		goto	    high_state
		
update_vals	btfsc	    sig_state,0
		incf	    sel_motor
		incf	    sig_state

		return

high_state	bcf	    status,c
		movfw	    indf		;Cargamos posicion
		addlw	    0xFA		;suma 250
		movwf	    ccpr1l		;Actualizamos ccpr1l
		movfw	    status
		andlw	    b'00000001'		;Enmascaramos el carry
		movwf	    ccpr1h		;Actualizamos ccpr1h
		
		; Ponemos pin correspondiente en alto
		movfw	    sel_motor		
		andlw	    b'00000011'		;Mascara para seleccion
		movwf	    motor_aux
		movlw	    b'00000001'
		movwf	    sel_pin
		call	    rotate_w_times
		movfw	    sel_pin
		iorwf	    portb,f
		goto	    update_vals
		
low_state	bcf	    status,c
		movfw	    indf
		sublw	    0xE8
		movwf	    ccpr1l
		movfw	    status
		andlw	    b'00000001'		;Enmascaramos el carry
		sublw	    b'00000011'		;restamos C si lo hay
		movwf	    ccpr1h		;Actualizamos ccpr1h

		; Ponemos pin correspondiente en bajo
		movfw	    sel_motor		
		andlw	    b'00000011'		;Mascara para seleccion
		movwf	    motor_aux
		movlw	    b'00000001'
		movwf	    sel_pin
		call	    rotate_w_times
		comf	    sel_pin,w
		andwf	    portb,f
		goto	    update_vals
		
		
rotate_w_times	bcf	    status,C
		btfsc	    status,Z
		return
		decf	    motor_aux
		rlf	    sel_pin,f
		goto	    rotate_w_times
		