# Este archivo guarda los datos separados por espacios.
# Hay que cambiar el codigo del TX.

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
    filePath = filePath.replace('dataSave - v2.py','saves')

    if not os.path.exists(filePath):
        os.makedirs(filePath)

    # Creating file
    dataDir = str(filePath) + '/' + time.strftime('%d%m') + time.strftime('%H-%M-%S_') + "Data" + ".csv"
    f = open(dataDir, "w")

else:
    print("Serial cerrado - No se puede crear el archivo")


#! ----- Main code -----
# Reading incoming data      
while serialInst.isOpen() == True:
    packet = serialInst.readline()
    packet = packet.split( )
    print(packet)
    # dataLists = []

    # timeCounter += 1
    # timeList.append(timeCounter)
    
    # Saving data on respective list
    # dataLists.append(timeCounter)
    # for pos,packets in enumerate(packet):
    #     dataLists.append(float(packets))
    
    # Serial data printing
    # print(dataLists)

    # Saving data to a file
    with open(dataDir, mode='a', newline="") as file:
        writer = csv.writer(file)
        writer.writerow(packet)

else:
    print("Serial cerrado - No se puede realizar el guardado de datos")
    