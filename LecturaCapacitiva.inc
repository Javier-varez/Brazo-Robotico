
;-------------------------------------------------;
;--------------- Lectura sensores ----------------;
;-------------------------------------------------;
;------- Inicialización -------;
init_read	bank0
		bcf	init_cap,0	; Borramos el bit de habilitación
		movlw   b'00000001'	; Elegimos el primer sensor a leer
		movwf   sensor

		bank1
		movlw   b'11110111'	; Ponemos todo como entradas menos aux
		movwf   trisa		
		bank0
		movlw   b'00001000'	; Elegimos el pin auxiliar (RA3)
		movwf   porta

		movlw   b'10000001'	; Activamos el ADC con prescaler 1/32
		iorwf	adcon0,f	

;------- Programa para los sensores -------;
cap_read	movlw	b'00001000'
		movwf	AuxADC
		call    comp_aux_adc	; Apuntamos al pin auxiliar del ADC
		
		comf	sensor,w	; Ponemos la línea del sensor a tierra
		bank1
		andwf	trisa,f
		
		bank0			; Volvemos a poner sensor como entrada 
		movfw	sensor
		bank1
		iorwf	trisa,f
		
		bank0
		movwf	AuxADC
		call	comp_aux_adc	; Apuntamos al sensor en el ADC
		
		movlw	b'00000010'	; Comenzamos la lectura
		iorwf	adcon0
		
wait		btfsc	adcon0,go_done  ; Esperamos a que finalice
		goto	wait
		
		call	selec_sens
		movwf	sensor_aux
		andwf	sensoresan,w	; Comprobamos el estado anterior del 
		btfss	status,z	; sensor
		goto	pulsado
		
no_pulsado	movlw	b'11110000'
		andwf	adresh,w		
		btfss	status,z	; Comprobamos si el sensor sigue sin 
		goto	end_read_sens	; pulsarse o se ha pulsado
		movfw	sensor_aux
		iorwf	sensoresan,f
		goto	end_read_sens
				
pulsado		movlw	b'11000000'
		andwf	adresh,w	
		btfsc	status,z	; Comprobamos si el sensor sigue pulsado
		goto	end_read_sens	; o se ha dejado de pulsar
		movfw	sensor_aux
		xorwf	sensoresan,f
		goto	end_read_sens
		
end_read_sens	movlw	b'00000100'	; Comprobamos si ya se han leido todos 
		andwf	sensor,w	; los sensores capacitivos
		btfss	status,z
		goto	pinza		; Si ya se han leido todos pasamos a ver
		rlf	sensor,f	; el estado de la pinza
		goto	cap_read
		
;----- Programa para la pinza -----;
pinza		movlw	b'10001101'	; Pin Aux como entrada ADC
		movwf	adcon0
		movlw	b'10010001'	; Elegimos pin de la pinza en ADC y 
		movwf	adcon0		; comenzamos la lectura
		
		bsf	adcon0,go_done
		
wait_p		btfsc	adcon0,go_done  ; Esperamos a que finalice
		goto	wait_p
		
		
		movfw	adresh
		sublw	0xE8
		btfss	status,c
		goto	abierta	

cerrada		bsf	sensoresan,0
		goto	fin
		
abierta		bcf	sensoresan,0
		
		;movlw	b'00000001'
		;andwf	sensoresan,w	; Comprobamos el estado anterior de la 
		;btfss	status,z	; pinza
		;goto	cerrada
		
;abierta		movlw	b'10000000'
;		andwf	adresh,w		
;		btfss	status,z	; Comprobamos si la pinza ha llegado 
;		goto	fin		; al máximo o no
;		bsf	sensoresan,0
;		goto	fin
				
;cerrada		movlw	b'10000000'
;		andwf	adresh,w	
;		btfsc	status,z	; Comprobamos si la pinza sigue en su
;		goto	fin		; máximo o se ha abierto
;		movlw	b'00000001'
;		bcf	sensoresan,0
;		goto	fin
		
fin		return
		
;-------------------------------------------------;
;------------------ Subrutinas -------------------;
;-------------------------------------------------;

;----- ADC -----;
comp_aux_adc	movlw	b'11111000'	; Comprobamos el valor de AuxADC
		addwf	AuxADC,w
		btfss	status,c
		goto	conf_mux_adc
		bcf	status,c	; Caso 1: AuxADC apunta al pin RA3
		movlw	b'00001100'
		iorwf	adcon0,w
		goto	carga_canal
		
conf_mux_adc	movlw	b'00000000'
		movwf	MuxADC
conf_mux_adc_bucle rrf	AuxADC,f	; Caso 2: AuxADC apunta al pin a leer
		btfsc	status,c
		goto	canal
		incf	MuxADC
		goto	conf_mux_adc_bucle
		
canal		bcf	status,c
		rlf	MuxADC,f
		rlf	MuxADC,f
		movlw	b'10000001'
		andwf	adcon0,w
		iorwf	MuxADC,w
		
carga_canal	movwf	adcon0	; Cargamos el pin auxiliar o el pin a 
		return			; leer en el ADC
		
;----- Selección del sensor que hemos leido -----;
selec_sens	clrf	select
		movfw	sensor
		movwf	sensor_aux
selec_sens_bucle rrf	sensor_aux,f
		btfsc	status,c
		goto	tabla
		incf	select
		goto	selec_sens_bucle

tabla		bcf	status,c
		movfw	select
		addwf	PCL,f
		retlw	b'00001000'
		retlw	b'00000100'
		retlw	b'00000010'
		