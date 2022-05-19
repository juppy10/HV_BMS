import serial
import csv
import codecs
import time

#Serial port outputs all cell voltages contiguously (e.g. 300003100035000...29000\r\n)
#see image for clarification

NUM_CELLS = 15

serialPort = serial.Serial(port="COM3", baudrate=115200, bytesize=8, timeout=10, stopbits=serial.STOPBITS_ONE)
serialString = ""  # Used to hold data coming over UART
count = 0

with open('balance_4.csv', 'w', newline='') as dataFile:
    dataWriter = csv.writer(dataFile)
    dataWriter.writerow(["time (ms)", "Cell 1", "Cell 2", "Cell 3", "Cell 4", "Cell 5", "Cell 6", "Cell 7", "Cell 8", "Cell 9", "Cell 10", "Cell 11", "Cell 12", "Cell 13", "Cell 14", "Cell 15", "Current", "SoC", "Temperature 1", "Temperature 2", "Temperature 3", "Temperature 4","UV\OV Fault"])
    start = time.time()

while 1:
    if serialPort.in_waiting > 0:
        cells=[]
        cells=[0 for i in range(NUM_CELLS+6)]
        #aaaa = serialPort.readline()
        #num1 = aaaa[0]

        for i in range(NUM_CELLS+6):
            rawData = serialPort.read(5)
            cells[i] = rawData.decode('Ascii')
        UV_OV_Flag = serialPort.read(1)
        UV_OV_Flag = UV_OV_Flag.decode('Ascii')
        endBits1 = serialPort.read(2)
        if(endBits1 == b'\r\n'):
            with open('balance_4.csv', 'a', newline='') as dataFile:
                dataWriter = csv.writer(dataFile)
                dataWriter.writerow([str(count), str(cells[0]), str(cells[1]), str(cells[2]), str(cells[3]), str(cells[4]), str(cells[5]),
                                     str(cells[6]), str(cells[7]) , str(cells[8]) , str(cells[9]),  str(cells[10]),  str(cells[11]),
                                     str(cells[12]),  str(cells[13]),  str(cells[14]),  str(cells[15]),  str(cells[16]), str(cells[17]), str(cells[18]), str(cells[19]), str(cells[20]), str(UV_OV_Flag)])
                print(round(count))
                print(cells[:NUM_CELLS])
                # if count > 10:
                #     end = time.time()
                #     print("Time:", end - start)
                #     break

        count = count + 0.1
