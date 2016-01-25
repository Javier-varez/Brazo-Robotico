
ini_eeprom	bank1
		bsf	    pie2,eeie	;Enable eeprom write interrupts
		bank0
		return
		
eeprom_read	bank2
		movwf	eeadr		;Address to read from
		bank3
		bcf	eecon1,eepgd	;Read from eeprom
		bsf	eecon1,rd	;Start read op
		bank2
		movfw	eedata		;Copy value to w
		bank0
		return
    
ISR_EEPROM	bank3
		bcf	eecon1,wren	;Disable write enable
		bank0
		bcf	pir2,eeif	;Clear flag
		
		return





