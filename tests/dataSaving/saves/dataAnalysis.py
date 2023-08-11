import matplotlib.pyplot as plt

# Pasa algo raro con el codigo ya que se guardan unos datos sin la fecha y hora del gps en la estacion terrena
# sin embargo, si se guardan todos los otros datos.
# En la SD no aparecen estos datos sin fecha y hora de la estacion terrena.


dataDir = 'sd data/data-volta_9-8-2023_23-12-46.txt'

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

# with open(dataDir, 'r') as file:
#     for line in file:
#         if line.isspace() == True:
#             pass
#         else:
#             line = line.strip()
#             lines = line.split(',')
            
#             if len(lines) > 1:                                 
#                 listTimes.append(lines[0][9:])
#                 listHumidity.append(lines[1])
#                 listTemperature.append(lines[2])
#                 listPressure.append(lines[3])
#                 listAltitude.append(lines[4])
#                 listAcelZ.append(lines[5])
#                 listmagTotal.append(lines[6])
#                 listheadDegrees.append(lines[7])
#                 listgasResistance.append(lines[8])
#                 listLat.append(lines[9])
#                 listLong.append(lines[10])
#                 listAltitudeGPS.append(lines[11])
#                 listhorizontalSpeed.append(lines[12])
#                 listwindRPM.append(lines[13])
#                 listwindDir.append(lines[14])
                

# plt.plot(listTimes, listmagTotal)
# plt.show()



with open(dataDir, 'r') as file:
    for line in file:
        line = line.strip()
        lines = line.split(' ')
        
        if len(lines) > 1:                                 
            listTimes.append(lines[0][9:])
            listHumidity.append(lines[1])
            listTemperature.append(lines[2])
            listPressure.append(lines[3])
            listAltitude.append(lines[4])
            listAcelZ.append(lines[5])
            listmagTotal.append(lines[6])
            listheadDegrees.append(lines[7])
            listgasResistance.append(lines[8])
            listLat.append(lines[9])
            listLong.append(lines[10])
            listAltitudeGPS.append(lines[11])
            listhorizontalSpeed.append(lines[12])
            listwindRPM.append(lines[13])
            listwindDir.append(lines[14])
            

plt.plot(listTimes, listAltitude)
plt.show()