'''
Codigo completamente probado con data guardada en SD y por estacion terrena.

INSTRUCCIONES
- Desde CMD dirigirse a la carpeta con el archivo que se desea analizar
- Revisar linea 66

FALTANTE
- Verificar unidades y datos en listas.
- Archivo para mostrar la similitud entre los datos de sd y estacion terrena. OPCIONAL
'''

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import time
import os


listTimes = []
listIAQ = []
listCO2 = []
listVOC = []
listPressure = []
listAltitude = []
listHeight = []
listTemperature = []
listHumidity = []
listFallSpeed = []
listAcelZ = []
listmagTotal = []
listheadDegrees = []
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


lists = [listTimes, listIAQ, listCO2, listVOC, listPressure, listAltitude, listHeight, listTemperature, listHumidity, listFallSpeed, 
         listAcelZ, listmagTotal, listheadDegrees, listLat, listLong, listAltitudeGPS, listhorizontalSpeed, listwindRPM, listwindDir]

listNames = ['NoUse', 'IAQ', 'CO2', 'VOC', 'Atmos [Pa]', 'Altitud [m]', 'Altura [m]', 'Temperatura [C]', 'Humedad [%]', 'Fall speed [ms]',
             'Acel en vertical [g]', 'Campo magnetico [uT]', 'Direccion norte [grados]', 'Latitud', 'Longitud', 'Altitud del GPS [m]',
             'Velocidad horizontal [kmh]', 'Direccion del viento']

listNames_nounits = ['NoUse', 'IAQ', 'CO2', 'VOC', 'Atmos', 'Altitud', 'Altura', 'Temperatura', 'Humedad', 'Fall speed',
             'Acel en vertical', 'Campo magnetico', 'Direccion norte', 'Latitud', 'Longitud', 'Altitud del GPS',
             'Velocidad horizontal', 'Direccion del viento']


# Creating folder           
mainPath = str(os.path.abspath(__file__))
mainPath = mainPath.replace('dataAnalysis.py','pdfs')

if not os.path.exists(mainPath):
    os.makedirs(mainPath)

dataFile = "250816-08-49_Data.txt"    #! ----- ARCHIVO A ANALIZAR -----

DATADIR = "dataSaving/saves/" + dataFile
FILEPATH = 'dataSaving/Resumen_' + time.strftime('%H-%M-%S') + '.txt'
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
        
        if (len(lines)-1):
            print(lines)    # Debugging
            listIAQ.append(float(lines[1]))
            listCO2.append(float(lines[2]))
            listVOC.append(float(lines[3]))
            listPressure.append(float(lines[4]))
            listAltitude.append(float(lines[5]))
            listHeight.append(float(lines[6]))
            listTemperature.append(float(lines[7]))
            listHumidity.append(float(lines[8]))
            listFallSpeed.append(float(lines[9]))
            listAcelZ.append(float(lines[10]))
            listmagTotal.append(float(lines[11]))
            listheadDegrees.append(float(lines[12]))
            listLat.append(float(lines[13]))
            listLong.append(float(lines[14]))
            listAltitudeGPS.append(float(lines[15]))
            listhorizontalSpeed.append(float(lines[16]))
            listwindRPM.append(float(lines[17]))
            # listwindDir.append(float(lines[18]))


#* ----- Code for time management -----
for _ in range(len(listHumidity)):
    times += 1
    listTimes.append(times)


#* ----- Plotting and analysis -----
with open(FILEPATH, 'w') as file:
    for pos, data in enumerate(lists):
        if pos==len(listNames):
            break
        
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
        
        # if pos!=5:
        nameFile = 'dataSaving/pdfs/' + 'Grafica_' + time.strftime('%H-%M-%S') + '_' + str(listNames[pos]) + '.pdf'
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