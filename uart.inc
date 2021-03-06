
;==== Asynchronous receiver/transmitter      ====;
;==== 9600 baud, no parity, 8bit data packet ====;
;==== 1 start bit and 1 stop bit	     ====;
;==== FOSC = 8 MHz, BRGH = 0, BFG16 = 0      ====;

;==== variables ====;
cblock		    0x50
current_data
wait_data
programming_mode
tx_soft_buffer
ctrl_mode
endc
		    
#define programming_code    0x66
#define ctrl_code	    0x6F
		    
;==== Inicialización ====;
init_uart	    bank3
		    clrf    baudctl	;BRG16 = 0, 8 bit mode
		    bank1
		    movlw   b'00100000'
		    movwf   txsta	;BRGH = 0, SYNC = 0, TXEN = 1
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
		    clrf    ctrl_mode
		    
		    return

;==== ISR UART ====;
ISR_RX_UART	    bank0
		    movfw   rcreg
		    movwf   current_data
		    
		    btfsc   programming_mode,0	;Si modo Progr. Entra en la rutina
		    goto    p_mode
		    
		    btfsc   ctrl_mode,0		;Si modo Ctrl. Entra en la rutina
		    goto    c_mode
		    
		    goto    enter_programming	;En caso contrario, activa modo
		    
p_mode		    movfw   current_data
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
		    bsf	    eecon1,wr	    ;write begins, it needs 2 cycles for writting properly
		    bank0
		    
		    return
		    
received_addr	    bank2
		    movwf   eeadr	    ;escribe direccion en eeadr
		    bank0
		    sublw   programming_code	    ;comprueba si es 66 para salir del
		    btfsc   status,Z	    ;modo programacion
		    goto    exit_programming
		    bsf	    wait_data,0
		    return

enter_programming   sublw   programming_code
		    btfss   status,Z
		    goto    enter_ctrl
		    bsf	    programming_mode,0
		    movlw   0xFF
		    call    send_data
		    return
		    
exit_programming    bcf	    programming_mode,0
		    movlw   0x80
		    call    send_data
		    return 
		    
enter_ctrl	    movfw   current_data
		    sublw   ctrl_code
		    btfss   status,Z
		    return
		    bsf	    ctrl_mode,0
		    movlw   0xF0
		    call    send_data
		    return
		    
c_mode		    sublw   ctrl_code
		    btfsc   status,Z
		    goto    exit_ctrl
		    movfw   current_data
		    andlw   0x0E
		    movwf   RemoteCtrls
		    return
		   
exit_ctrl	    bcf	    ctrl_mode,0
		    movlw   0x8F
		    call    send_data
		    return 
		    
; Subrutina para enviar datos por UART
send_data	    bank1
		    btfsc   txsta,trmt		;Chequea si está ocupado el TX
		    goto    send_inmediately
		    goto    enable_tx_interrupt
		    
enable_tx_interrupt bsf	    pie1,txie
		    bank0
		    movwf   tx_soft_buffer
		    return
		    
send_inmediately    bank0
		    movwf   txreg
		    return
		    
;TX_ISR
		    
ISR_TX_UART	    bank0
		    movfw   tx_soft_buffer  ;Obtiene el dato a enviar
		    movwf   txreg	    ;Envia el dato
		    bank1
		    bcf	    pie1,txie	    ;Desactiva la interrupcion de TX
		    bank0
		    return
		    