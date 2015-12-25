		
;---------------------------------;
;---- Variables ------------------;
;---------------------------------;

		cblock	0x30
SensoresAnDelayed
		endc

;--- Definiciones ---;
#define left		SensoresAn,3
#define change		SensoresAn,2
#define right		SensoresAn,1
#define pinza_c		SensoresAn,0
#define left_sensor	portb,4
#define right_sensor	portb,5
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
		btfss	right_sensor	    ;Comprueba si el sensor está a 1
		goto	CheckIz
		btfss	right		    ;Comprueba que se haya pulsado Derecha/Abajo +
		goto	CheckIz
		goto	MotorDec
		

MotorDec
		movf	MotorSelect,w	    ;Mueve el contenido del registro al W
		addlw	pos1		    ;Suma motor_actual+pos1 para apuntar a la direcci?n del motor actual
		movwf	fsr		    ;Pone en FSR la direcci?n que apunta al registro de posici?n al motor actual		
		
		movfw	indf
		btfss	status,Z	    ;Si la posicion es 0, no decrementa
		decf	indf,f
		goto	CheckIz
		
;---------------------------------;
;----Izquierda/Arriba-------------;
;---------------------------------;
	
CheckIz
		btfss	left_sensor	    ;Comprueba si el sensor está a 1
		goto	storeSensAn
		btfss	left		    ;Comprueba que se haya pulsado Izquierda/Arriba +
		goto	storeSensAn
		goto	MotorInc
		
MotorInc	
		movf	MotorSelect,w	    ;Mueve el contenido del registro al W
		addlw	pos1		    ;Suma motor_actual+pos1 para apuntar a la direcci?n del motor actual
		movwf	fsr		    ;Pone en FSR la direcci?n que apunta al registro de posici?n al motor actual		
		
		comf	indf,w
		btfss	status,Z	    ;Si la posicion es 0, no incrementa
		incf	indf,f
		goto	storeSensAn


storeSensAn	movfw	SensoresAn
		movwf	SensoresAnDelayed
		movfw	MotorSelect
		movwf	portc
		return