'''
Codigo completamente probado con data guardada en SD y por estacion terrena.

FALTANTE
- Verificar unidades y datos en listas.
- Archivo para mostrar la similitud entre los datos de sd y estacion terrena.
'''

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import time
import os


listTimes = []
listHumidity = []
listTemperature = []
listPressure = []
listAltitude = []
listAcelZ = []
listmagTotal = []
listheadDegrees = []
listgasResistance = []
listLat = []
listLong = []
listAltitudeGPS = []
listhorizontalSpeed = []
listwindRPM = []
listwindDir = []

listdataMax = []
listdataMin = []
listdataMean = []
listDatas = []

lists = [listHumidity, listTemperature, listPressure, listAltitude, listAcelZ, listmagTotal,
         listheadDegrees, listgasResistance, listLat, listLong, listAltitudeGPS, listhorizontalSpeed,
         listwindRPM, listwindDir]

listNames = ['Humedad [%]', 'Temperatura [C]', 'Presion atmos [hPa]', 'Altitud [m]',
             'Aceleracion en eje vertical [g]', 'Campo magnetico [uT]', 'Direccion norte [degrees]',
             'Gas CAMBIAR', 'Latitud', 'Longitud', 'Altitud del GPS [m]', 'Velocidad horizontal [m s]',
             'Velocidad del viento [m s]', 'Direccion del viento']

listNames_nounits = ['Humedad', 'Temperatura', 'Presion atmos', 'Altitud',
             'Aceleracion en eje vertical', 'Campo magnetico', 'Direccion norte',
             'Gas CAMBIAR', 'Latitud', 'Longitud', 'Altitud del GPS', 'Velocidad horizontal',
             'Velocidad del viento', 'Direccion del viento']
#! VERIFICAR UNIDADES Y DATOS


# Creating folder           
filePath = str(os.path.abspath(__file__))
filePath = filePath.replace('dataAnalysis.py','pdfs')

if not os.path.exists(filePath):
    os.makedirs(filePath)

dataFile = "data-volta_9-8-2023_23-16-13.txt"    #! ----- ARCHIVO A ANALIZAR -----

DATADIR = "sd_data/" + dataFile
FILEPATH = 'Resumen_' + time.strftime('%H-%M-%S') + '.txt'
times = 0



#* ----- Reading Data -----
with open(DATADIR, 'r') as file:   
    for line in file:
        linesdata = [0]
        line = line.strip()
        
        # ----- Data cleaning -----
        if line.find(',') != -1:
            line = line.replace(',', ' ')
        if line.find('"') != -1:
            line = line.replace('"', '')
        lines = line.split(' ')
        
        # ----- Bad data detection and removing -----
        for value in lines:
            try:
                float(value)
                linesdata.append(value)
                lines = linesdata
            except:
                continue
        
        if len(lines)==15:      #! Cambiar segun tamano de nueva cadena de datos
            print(lines)    # Debugging
            listHumidity.append(float(lines[1]))
            listTemperature.append(float(lines[2]))
            listPressure.append(float(lines[3]))
            listAltitude.append(float(lines[4]))
            listAcelZ.append(float(lines[5]))
            listmagTotal.append(float(lines[6]))
            listheadDegrees.append(float(lines[7]))
            listgasResistance.append(float(lines[8]))
            listLat.append(float(lines[9]))
            listLong.append(float(lines[10]))
            listAltitudeGPS.append(float(lines[11]))
            listhorizontalSpeed.append(float(lines[12]))
            listwindRPM.append(float(lines[13]))
            listwindDir.append(float(lines[14]))


#* ----- Code for time management -----
for _ in range(len(listHumidity)):
    times += 1
    listTimes.append(times)


#* ----- Plotting and analysis -----
with open(FILEPATH, 'w') as file:
    for pos, data in enumerate(lists):
        plot1, = plt.plot(listTimes, listAltitude, color='r')
        plt.ticklabel_format(useOffset=False)
        plt.xlabel("Tiempo [s]", weight='bold')
        plt.ylabel("Altitud [m]", weight='bold')
        plt.twinx()
        
        dataMax = max(data)
        dataMin = min(data)
        dataMean = np.mean(data)
        listdataMax.append(dataMax)
        listdataMin.append(dataMin)
        listdataMean.append(dataMean)
        lineMax = plt.axhline(y=dataMax, linestyle='--', color='midnightblue', linewidth='1')
        lineMin = plt.axhline(y=dataMin, linestyle='-.', color='midnightblue', linewidth='1')
        lineMean = plt.axhline(y=dataMean, linestyle=':', color='midnightblue', linewidth='1')
        
        plot2, = plt.plot(listTimes, data)
        plt.ylabel(listNames[pos], weight='bold')

        plt.legend([plot1, plot2, lineMax, lineMin, lineMean],
                ["Altitud", listNames_nounits[pos], "Maximo", "Minimo", "Promedio"])
        plt.tight_layout()
        
        if pos!=3:
            nameFile = 'pdfs/' + 'Grafica_' + time.strftime('%H-%M-%S') + '_' + str(listNames[pos]) + '.pdf'
            plt.savefig(nameFile)
            # plt.show()
        plt.clf()
    
    #! ----- Resume data saving -----
    listDatas.append(listdataMax)
    listDatas.append(listdataMin)
    listDatas.append(listdataMean)
    
    dataFrame = pd.DataFrame(listDatas, index=['Max', 'Min', 'Mean'], columns=[listNames])
    dataFrame = dataFrame.transpose()
    file.write(str(dataFrame))