
cblock	0x60
	RemoteCtrls
endc
	
remote_init	clrf		RemoteCtrls
		return
    
remotePos	bank0
		movfw		sensoresan
		andlw		b'00000001'
		iorwf		RemoteCtrls,w
		movwf		sensoresan
		return

