
;------------------;	
;--- Subrutinas ---;
;------------------;

ini_osc		movlw	    0x70
		bank1
		movwf	    osccon	;8 MHz
		bank0
		return
		
ini_es		bank1
		movlw	    b'11110000'	; Pines de los motores como salidas
		movwf	    trisb
		movlw	    b'11111100'	; Ponemos LED's como salida
		movwf	    trisc
		bank3
		clrf	    anselh	; Ponemos el puerto B como digital
		bank0
		return