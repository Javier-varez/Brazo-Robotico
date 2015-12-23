
;-------------------------------------------------;
;---------------- Inicialización -----------------;
;-------------------------------------------------;    
in_timer0   clrwdt
	    bank0
	    clrf    tmr0	    ;Ponemos timer0 como temporizador,
	    bank1		    ;seleccionamos el prescaler para el timer 
	    movlw   b'11000000'	    
	    andwf   option_reg,w
	    iorlw   b'00000101'	    ;Prescaler a 1:64
	    movwf   option_reg
	       
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



