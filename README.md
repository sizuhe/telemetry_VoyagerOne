# Voyager One

These codes describe how our CanSat *VoyagerOne* electronics works. Made for the Latin American Space Challenge (*LASC*) competition.

This project was developed in University of Antioquia by [Voyager](https://grupoastra.github.io/voyager/) student group.


## Main operation
### Modules used
These are the modules used in our CanSat and its main mission objectives for data measuring.

- **BME680**: Temperature, pressure, altitude and volatile organic compounds (VOCs).
- **IMU GY85**: Accelerometer, gyroscope and magnetometer.
- **HC 020K**: Encoder used for anemometer rpm calculations.
- **LoRa SX1278**: Long range communication.
- **SD Module**: On board data saving.
- **GPS NEO6M**: Position and horizontal speed.


### dataSaving folder
`dataSave.py' file is able to automatically detect an Arduino and read data coming from serial port using `PySerial', then it saves that data to a .txt file differentiated by date and time. If theres any disconnection between transmitter and receptor it tries to recover the signal.

`dataAnalysis.py' file takes data from a manually configured file and reads all the data packet, creates a main summary and plots all the data independently.