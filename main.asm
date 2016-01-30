;======================================================================;
; Proyecto:	Brazo Robótico
; Nombres:	Francisco Javier Álvarez García
;		Juan Manuel Álvarez Toribio
;		Elena González Fernández
;		Rubén Haba Pascual 
; MCU:		PIC16F883
; Fecha:	28/10/2015
;======================================================================;
; Descripción:
;		Código para el control y manejo del brazo robótico
;		de 3 grados de libertad con deteccion de colisiones
;		y agarre de la pinza.
;======================================================================;
    
    
#include <p16f883.inc>

; Reloj interno, watchdog desactivado y programación de bajo voltaje desactivada
; CONFIG1
; __config 0x2BF4
 __CONFIG _CONFIG1, _FOSC_INTRC_NOCLKOUT & _WDTE_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_ON & _IESO_OFF & _FCMEN_ON & _LVP_OFF
; CONFIG2
; __config 0x3FFF
 __CONFIG _CONFIG2, _BOR4V_BOR40V & _WRT_OFF

#include "bancos.inc"
		
;-----------------;	    
;--- Variables ---;
;-----------------;
		
		cblock	    0x20
init_cap		    ;0x20
sensor			    ;0x21
AuxADC			    ;0x22
MuxADC			    ;0x23
select			    ;0x24
sensor_aux		    ;0x25
sensoresan		    ;0x26
pos1			    ;0x27				    
pos2			    ;0x28			    
pos3			    ;0x29			    
pos4			    ;0x2A	
MotorSelect		    ;0x2B
timer0_prescaler	    ;0x2C
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
;--- Vector de Interrupción ---;
;------------------------------;
		org	    0x04
interruptv	goto	    ISR
		
;--------------------------------;
;--- Rutina de inicialización ---;
;--------------------------------;
		org	    0x05
init		call	    ini_osc
		call	    ini_es
		call	    ini_eeprom	
		call	    ini_vars	;Inicializa posicion inicial y sentidos (archivo decisiones.inc)
		call	    init_motors	;Inicializa las variables relativas a los motores y SFR's
		call	    in_timer0	;Inicializa el timer 0
		call	    init_uart	;Inicializa la UART
		call	    remote_init ;Inicializa Remote Ctrl
		bsf	    intcon,peie	;Habilitamos interrupciones de perifericos
		bsf	    intcon,gie	;Habilitamos interrupciones
		
bucle_ppal	btfss	    init_cap,0	; Comprobamos si necesitamos actualizar
		goto	    bucle_ppal
		call	    init_read	; Lectura capacitiva de sensores
		btfsc	    ctrl_mode,0
		call	    remotePos	; Posicion remota
		call	    ChangeMotor	; Actualizacion de posicion
		goto	    bucle_ppal
		
;---------------------------------;
;--- Interrupt Service Routine ---;
;---------------------------------;
ISR		movwf	    w_prev	;Almacenamos el acumulador
		swapf	    status,w	;Cargamos el registro de estado
		movwf	    status_prev	;Almacenamos el registro de estado
		
		btfsc	    pir1,ccp1if	
		call	    ISR_CCP1	;CCP1
		btfsc	    intcon,t0if
		call	    isr_timer	;timer0
		btfsc	    pir2,eeif
		call	    ISR_EEPROM  ;EEPROM
		btfsc	    pir1,rcif
		call	    ISR_RX_UART	;UART RX
		bank1
		btfss	    pie1,txie	;Si esta activa la interrucion de TX
		goto	    exit_isr
		bank0
		btfsc	    pir1,txif
		call	    ISR_TX_UART	;UART TX
		
exit_isr	swapf	    status_prev,w
		movwf	    status	;Restauramos el vector de estado
		swapf	    w_prev,f	
		swapf	    w_prev,w	;Restauramos w sin modificar status
		retfie			;Volvemos de la interrupción
		
		
;----------------;		
;--- Includes ---;
;----------------;
#include "inicializaciones.inc"
#include "Timer0.inc"
#include "LecturaCapacitiva.inc"
#include "decisiones.inc"
#include "ControlMotores.inc"
#include "uart.inc"
#include "eeprom.inc"
#include "RemoteCtrl.inc"
		end


