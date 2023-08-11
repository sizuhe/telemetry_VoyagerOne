# Archivos guardados por separacion de comas

import serial.tools.list_ports
import os
import time
import csv



serialInst = serial.Serial()
serialInst.baudrate = 9600
timeCounter = 0
arduinoList = ['Arduino', 'CH340']
timeList = []


#! ----- Serial detection -----
#* ----- Auto mode -----
if len(serial.tools.list_ports.comports()) != 0:
            for port in serial.tools.list_ports.comports():
                for arduino in arduinoList:
                    if port.description.find(arduino) != -1:
                        serialInst.port = port.name
                        arduinoName = port.description
                        serialInst.open()
                        break

#* ----- Manual mode -----
# print('Puertos serial conectados:',*serial.tools.list_ports.comports())
# serialInst.port = 'COM3'
# serialInst.open()


#! ----- Folder and file creation -----
if serialInst.isOpen() == True:
    # Creating folder           
    filePath = str(os.path.abspath(__file__))
    filePath = filePath.replace('dataSave - v1.py','saves')

    if not os.path.exists(filePath):
        os.makedirs(filePath)

    # Creating file
    dataDir = str(filePath) + '/' + time.strftime('%d%m') + time.strftime('%H-%M-%S_') + "Data" + ".txt"

else:
    print("Serial cerrado - No se puede crear el archivo")


#! ----- Main code -----
# Reading incoming data      
while True:
    try:
        packet = serialInst.readline()
        packet = packet.decode("utf-8")     
        packet = packet.split(' ')
        dataLists = []

        for packets in packet:
            dataLists.append(packets)
        
        # Serial data printing
        print(dataLists)

        # Saving data to a file
        with open(dataDir, mode='a', newline="") as file:
            writer = csv.writer(file)
            writer.writerow(dataLists)

    except:
        packet = serialInst.readline()
        packet = packet.decode("utf-8")     
        packet = packet.split(' ')
        print("Perdida de conexión\nRecuperando señal... ... ...")