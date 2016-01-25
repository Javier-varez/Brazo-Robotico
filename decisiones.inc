		
;---------------------------------;
;---- Variables ------------------;
;---------------------------------;

		cblock	0x30
SensoresAnDelayed
invert_1
invert_2
invert_3
invert_4
		endc

;--- Definiciones ---;
#define left		SensoresAn,3
#define change		SensoresAn,2
#define right		SensoresAn,1
#define pinza_c		SensoresAn,0
#define left_sensor	portb,4
#define right_sensor	portb,5

;---------------------------------;
;---- Inicializaciones -----------;
;---------------------------------;
		
ini_vars	; Lectura inicial de sensores = 0
		clrf	    SensoresAn
		clrf	    SensoresAnDelayed
		; Posicion inicial de los motores
		movlw	    0x00
		call	    eeprom_read
		movwf	    pos1
		movlw	    0x01
		call	    eeprom_read
		movwf	    pos2
		movlw	    0x02
		call	    eeprom_read
		movwf	    pos3
		movlw	    0x03
		call	    eeprom_read
		movwf	    pos4
		; Motor seleccionado = 0
		clrf	    MotorSelect	
		; Invertir movimientos
		movlw	    0x04
		call	    eeprom_read
		movwf	    invert_1
		movlw	    0x05
		call	    eeprom_read
		movwf	    invert_2
		movlw	    0x06
		call	    eeprom_read
		movwf	    invert_3
		movlw	    0x07
		call	    eeprom_read
		movwf	    invert_4
		return
		
;---------------------------------;
;---- Motor Select ---------------;
;---------------------------------;
ChangeMotor	
		movfw	SensoresAnDelayed
		xorwf	SensoresAn,w
		andlw	b'00000100'
		btfsc	status,Z	    ;Si ha cambiado el estado del boton de cambio, salta
		goto	CheckDer
		btfss	change		    ;Comprueba si ha sido un flanco de subida
		goto	CheckDer
	        incf	MotorSelect	    ;Incrementa la variable
		btfss	MotorSelect,2	    ;Comprueba que no se haya 'desbordado'
		goto	CheckDer
		clrf	MotorSelect
		
;---------------------------------;
;----Derecha/Abajo----------------;
;---------------------------------;

CheckDer
		movf	MotorSelect,f	    ;Z = 1 si MotorSelect es 0
		btfss   status,Z	    ;Si Z = 1 salta
		goto	CheckSensAnDer
		btfsc	right_sensor	    ;Si el sensor esta a 1, inmoviliza el robot
		goto	CheckIz		    ;Chequea el boton izq
CheckSensAnDer	btfss	right		    ;Comprueba que se haya pulsado Derecha/Abajo +
		goto	CheckIz
		movf	MotorSelect,w
		addlw	invert_1	    ;Direcciona el invert correspondiente
		movwf	fsr
		btfsc	indf,0		    ;Salta si invert_x[0] == 0
		goto	DerInvert
		call	MotorDec
		goto	CheckIz

DerInvert	call	MotorInc
		goto	CheckIz
		
;---------------------------------;
;----Izquierda/Arriba-------------;
;---------------------------------;
	
CheckIz
		movf	MotorSelect,f	    ;Z = 1 si MotorSelect es 0
		btfss   status,Z	    ;Si Z = 1 salta
		goto	CheckPinza
		btfsc	left_sensor	    ;Si el sensor esta a 1, inmoviliza el robot
		goto	storeSensAn	    ;Almacena el estado actual

CheckPinza	movfw	MotorSelect
		sublw	b'00000011'	    ;Comprobamos Si MotorSel es la pinza
		btfss	status,Z
		goto	CheckSensAnIzq
		btfsc	pinza_c
		goto	storeSensAn

CheckSensAnIzq	btfss	left		    ;Comprueba que se haya pulsado Izquierda/Arriba +
		goto	storeSensAn
		movf	MotorSelect,w
		addlw	invert_1	    ;Direcciona el invert correspondiente
		movwf	fsr
		btfsc	indf,0		    ;Salta si invert_x[0] == 0
		goto	IzqInvert
		call	MotorInc
		goto	storeSensAn

IzqInvert	call	MotorDec
		goto	storeSensAn

storeSensAn	movfw	SensoresAn
		movwf	SensoresAnDelayed
		movfw	MotorSelect
		movwf	portc
		return
		
;==== Subrutinas ====;
		

MotorDec
		movf	MotorSelect,w	    ;Mueve el contenido del registro al W
		addlw	pos1		    ;Suma motor_actual+pos1 para apuntar a la direcci?n del motor actual
		movwf	fsr		    ;Pone en FSR la direcci?n que apunta al registro de posici?n al motor actual		
		
		movfw	indf
		btfss	status,Z	    ;Si la posicion es 0, no decrementa
		decf	indf,f
		return
		
MotorInc	
		movf	MotorSelect,w	    ;Mueve el contenido del registro al W
		addlw	pos1		    ;Suma motor_actual+pos1 para apuntar a la direcci?n del motor actual
		movwf	fsr		    ;Pone en FSR la direcci?n que apunta al registro de posici?n al motor actual		
		
		comf	indf,w
		btfss	status,Z	    ;Si la posicion es 0, no incrementa
		incf	indf,f
		return