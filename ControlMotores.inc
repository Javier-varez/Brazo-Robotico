
;-----------------;
;--- Variables ---;
;-----------------;		

		cblock	    0x40
sel_motor
sig_state
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
		
		clrf	sig_state
		clrf	sel_motor
		
		; Habilitar interrupciones, modulo CCP1
		bank1
		bsf	pie1,ccp1ie		;habilitamos la interrupcion del modulo
		bank0
		
		return
		
;--- CCP1 ISR ---;
		
ISR_CCP1	bank0
		clrf	    tmr1h		;ponemos a 0 el timer1
		clrf	    tmr1l
		
		;Seleccion del motor
		movfw	    sel_motor
		andlw	    b'00000011'		;Mascara para seleccion
		addlw	    pos1
		movwf	    fsr			;Seleccion direccionamiento indirecto
		
		btfsc	    sig_state,0
		goto	    low_state
		goto	    high_state
		
update_vals	btfsc	    sig_state,0
		incf	    sel_motor
		incf	    sig_state
		
		bcf	    pir1,ccp1if		;Borramos la bandera
		
		return

high_state	bcf	    status,c
		clrf	    ccpr1h		
		rlf	    indf,w		;Multiplicamos por 2 posx
		btfsc	    status,c
		incf	    ccpr1h,f
		addlw	    0x7D		;suma 125 (0.5ms)
		movwf	    ccpr1l		;Actualizamos ccpr1l
		movfw	    status
		andlw	    b'00000001'		;Enmascaramos el carry
		addwf	    ccpr1h,f		;Actualizamos ccpr1h
		
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
		
low_state	;Se debe restar a 4.5 ms (0x465) el valor contenido en indf
		;Comenzamos con el calculo de la parte baja del registro
		bcf	    status,c
		;el valor del registro va multiplicado por 2 para conseguir el rango
		rlf	    indf,w		
		sublw	    0x65		;Restamos la parte baja de 0x465
		movwf	    ccpr1l		;Actualizamos ccpr1l
		
		;Se sigue con la parte alta del registro (0x4 - borrow - indf[7])
		movfw	    status		
		andlw	    b'00000001'		;Enmascaramos el carry
		xorlw	    b'00000001'		;Invertimos el carry (borrow)
		sublw	    0x04		;Restamos ~C
		movwf	    ccpr1h		;Actualizamos ccpr1h

		;restamos a ccpr1h el valor de indf[7]
		bcf	    status,c
		rlf	    indf,w
		movfw	    ccpr1h
		btfsc	    status,c
		addlw	    0xFF		;igual a restar 1
		movwf	    ccpr1h		;se actualiza CCPR1H con el valor final
		
		

		; Ponemos pin correspondiente en bajo
		movfw	    sel_motor		
		andlw	    b'00000011'		;Mascara para seleccion
		movwf	    motor_aux
		movlw	    b'00000001'
		movwf	    sel_pin		;Selecciona el bit del puerto B 
		call	    rotate_w_times	;correspondiente al motor
		comf	    sel_pin,w		;Lo invierte para ponerlo a cero
		andwf	    portb,f		;utilizando un and.
		goto	    update_vals
		
;Rota sel_pin a la izquierda las veces establecidas en W
rotate_w_times	bcf	    status,C
		btfsc	    status,Z
		return
		decf	    motor_aux
		rlf	    sel_pin,f
		goto	    rotate_w_times
		