
;==== Asynchronous receiver/transmitter      ====;
;==== 9600 baud, no parity, 8bit data packet ====;
;==== 1 start bit and 1 stop bit	     ====;
;==== FOSC = 8 MHz, BRGH = 0, BFG16 = 0      ====;

;==== variables ====;
cblock		    0x50
wait_data
programming_mode
endc
		    
;==== Inicialización ====;
init_uart	    bank3
		    clrf    baudctl	;BRG16 = 0, 8 bit mode
		    bank1
		    clrf    txsta	;BRGH = 0, SYNC = 0, TXEN = 0
		    movlw   0x0C	;Set to 9600 bauds
		    movwf   spbrg
		    bank0
		    movlw   b'10010000'	;Enable serial port pins (TX and RX)
		    movwf   rcsta	;SPEN = 1
		    
		    ;Enable RX interrupts
		    bank1
		    bsf	    pie1,rcie	;habilitamos la interrupcion del modulo
		    bank0
		    
		    bsf	    rcsta,cren
		    clrf    wait_data	;inicializa variable wait_data
		    clrf    programming_mode	;modo de programacion desactivado
		    
		    return

;==== ISR UART ====;
ISR_UART	    bank0
		    movfw   rcreg
		    
		    btfss   programming_mode,0	;si no esta activado el modo
		    goto    enter_programming	;programacion, intenta activarlo
		    
		    btfss   wait_data,0		;en funcion de estar esperando
		    goto    received_addr	;dato o no, salta a la rutina correspondiente
		    
received_data	    bcf	    wait_data,0		;elimina wait_data, para esperar addr
		    bank2
		    movwf   eedata		;escribe el dato en eedata
		    bank3
		    bcf	    eecon1,eepgd    ;Write EEPROM
		    bsf	    eecon1,wren	    ;Enable write
		    
		    ;required sequence
		    movlw   0x55
		    movwf   eecon2
		    movlw   0xAA
		    movwf   eecon2
		    bsf	    eecon1,wr	    ;write begins
		    bank0
		    
		    return
		    
received_addr	    bank2
		    movwf   eeadr	    ;escribe direccion en eeadr
		    bank0
		    sublw   0x66	    ;comprueba si es 66 para salir del
		    btfsc   status,Z	    ;modo programacion
		    goto    exit_programming
		    bsf	    wait_data,0
		    return

enter_programming   sublw   0x66
		    btfsc   status,Z
		    bsf	    programming_mode,0
		    return
		    
exit_programming    bcf	    programming_mode,0
		    return