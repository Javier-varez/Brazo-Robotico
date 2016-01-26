#!/usr/bin/python
# -*- coding: utf-8 -*-
import serial
import serial.tools.list_ports
import os
import sys

# Constantes
switch_programming = 0x66
enter_programming_ack = 0xFF
exit_programming_ack = 0x80
# Funciones

clear = lambda: os.system('clear')

def enter_programming_mode():
    ser.write(chr(switch_programming))
    response = ser.read(1)
    if response==chr(enter_programming_ack):
        print('Comms channel opened.');
    elif response==chr(exit_programming_ack):
        enter_programming_mode()
    else:
        print('Programming mode couldn\'t be stablished. Exiting now... ')
        sys.exit()



def write_EEPROM(addr,data):
    ser.write(chr(addr))
    ser.write(chr(data))
    written_val = ser.read(1)
    if written_val==chr(data):
        print('Data written to EEPROM.')
        print('\tAddr: '+hex(addr))
        print('\tData: '+hex(data)+'\n')
    else:
        print('Error! Write op failed. Data = ' + hex(written_val))

def exit_programming_mode():
    ser.write(chr(switch_programming))
    response = ser.read(1)
    if response==chr(exit_programming_ack):
        print('Comms channel closed.')
    elif response==chr(enter_programming_ack):
        exit_programming_mode()
    else:
        print('There was a problem communicating with the device. Exiting now...')
        sys.exit()

# Main code
clear()

print('')
print(u'=====================================')
print(u'======== Brazo Robótico V1.0 ========')
print(u'===== Software de configuración =====')
print(u'=====================================')
print('')

# Show all available ports to choose from
ports = serial.tools.list_ports.comports()

i = 0
puertos = []
print('Available ports:')
for port in ports:
    print('[' + str(i) + ']: ' + port.device)
    puertos.append(port)
    i = i + 1

index = int(raw_input('Insert destination port: '))

ser = serial.Serial();

ser.port = (puertos[index]).device
ser.baudrate = 9600
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
ser.timeout = 1

ser.open();

print('')
addr = int(raw_input('\tAddr: '),16)
data = int(raw_input('\tData: '),16)
print(u'=====================================\n')
print('Attempting write to EEPROM:')
print('')

enter_programming_mode()
write_EEPROM(addr,data)
exit_programming_mode()

ser.close()
