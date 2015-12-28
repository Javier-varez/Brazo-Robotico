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
		