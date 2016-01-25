	    
;-------------------------------------------------;
;---------------- Inicialización -----------------;
;-------------------------------------------------;    
in_timer0   clrwdt
	    bank0
	    clrf    tmr0	    ;Ponemos timer0 como temporizador,
	    movlw   0x08
	    call    eeprom_read
	    andlw   b'00000111'
	    movwf   timer0_prescaler
	    bank1		    ;seleccionamos el prescaler para el timer 
	    movlw   b'11000000'	    
	    andwf   option_reg,w
	    bank0
	    iorwf   timer0_prescaler,w ;actualizamos con el prescaler programado
	    bank1
	    movwf   option_reg	    ;Inicializamos timer 0
		
;---- Habilitar interrupción del timer0 ----;	    
	    bank0
	    bsf	    intcon,t0ie
	    
	    return

;-------------------------------------------------;
;------------- Rutina de interrupción ------------;
;-------------------------------------------------;	    
isr_timer   bank0
	    bsf	    init_cap,0
	    
	    bcf	    intcon,t0if	    ;Borramos bandera de timer0
	    return  



