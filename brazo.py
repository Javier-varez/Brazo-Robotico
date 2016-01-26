#!/usr/bin/python
# -*- coding: utf-8 -*-
import serial
import serial.tools.list_ports
import os

# Funciones

clear = lambda: os.system('clear')

def enter_programming_mode(addr,data):
    ser.write(chr(0x66))
    response = ser.read(1)
    if response==chr(0xFF):
        print('Comms channel opened.');
        write_EEPROM(addr,data)
        exit_programming_mode()
    elif response==chr(0x80):
        enter_programming_mode()



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
    ser.write(chr(0x66))
    if ser.read(1)==chr(0x80):
        print('Comms channel closed.')

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

enter_programming_mode(addr,data)
