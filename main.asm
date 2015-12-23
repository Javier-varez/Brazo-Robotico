;======================================================================;
; Proyecto:	Brazo Rob�tico
; Nombres:	Francisco Javier �lvarez Garc�a
;		Juan Manuel �lvarez Toribio
;		Elena Gonz�lez Fern�ndez
;		Rub�n Haba Pascual 
; MCU:		PIC16F883
; Fecha:	28/10/2015
;======================================================================;
; Descripci�n:
;		C�digo para el control y manejo del brazo rob�tico.
;======================================================================;
    
    
#include <p16f883.inc>

; Reloj interno, watchdog desactivado y programaci�n de bajo voltaje desactivada
; CONFIG1
; __config 0x2BF4
 __CONFIG _CONFIG1, _FOSC_INTRC_NOCLKOUT & _WDTE_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_ON & _IESO_OFF & _FCMEN_ON & _LVP_OFF
; CONFIG2
; __config 0x3FFF
 __CONFIG _CONFIG2, _BOR4V_BOR40V & _WRT_OFF
 
;--- Direccionamiento directo para el banco 0 ---    
bank0		macro
		bcf	    status,rp0
		bcf	    status,rp1
		endm

;--- Direccionamiento directo para el banco 1 ---    
bank1		macro
		bsf	    status,rp0
		bcf	    status,rp1
		endm

;--- Direccionamiento directo para el banco 2 ---    
bank2		macro
		bcf	    status,rp0
		bsf	    status,rp1
		endm
	    
;--- Direccionamiento directo para el banco 3 ---    
bank3		macro
		bsf	    status,rp0
		bsf	    status,rp1
		endm
		
		
;-----------------;	    
;--- Variables ---;
;-----------------;
		
		cblock	    0x20
init_cap
sensor
AuxADC
MuxADC
select
sensor_aux
sensoresan
		endc
		
;-------------------------;
;--- Almacen de Estado ---;
;-------------------------;
		cblock	    0x70
w_prev
status_prev
		endc

;-----------------------;
;--- Vector de reset ---;
;-----------------------;
		
		org	    0x00
resetv		goto	    init

;------------------------------;
;--- Vector de Interrupci�n ---;
;------------------------------;
		org	    0x04
interruptv	goto	    ISR
		
;--------------------------------;
;--- Rutina de inicializaci�n ---;
;--------------------------------;
		org	    0x05
init		movlw	    0x70
		bank1
		movwf	    osccon	    ;8 MHz
		clrf	    trisc	    ;Recordar borrar instruccion 
		bank0
		clrf	    sensoresan
		call	    in_timer0
		bsf	    intcon,gie
		
bucle_ppal	btfsc	    init_cap,0
		call	    init_read
		goto	    bucle_ppal
;---------------------------------;
;--- Interrupt Service Routine ---;
;---------------------------------;
ISR		movwf	    w_prev	;Almacenamos el acumulador
		swapf	    status,w	;Cargamos el registro de estado
		movwf	    status_prev	;Almacenamos el registro de estado
		
		btfsc	    intcon,t0if
		call	    isr_timer	;Procesamiento necesario
		
		swapf	    status_prev,w
		movwf	    status	;Restauramos el vector de estado
		swapf	    w_prev,f	
		swapf	    w_prev,w	;Restauramos w sin modificar status
		retfie			;Volvemos de la interrupci�n
		
		
;------------------;	
;--- Subrutinas ---;
;------------------;

		
		
;----------------;		
;--- Includes ---;
;----------------;
#include "Timer0.inc"
#include "LecturaCapacitiva.inc"
#include "ControlMotores.inc"

		end


