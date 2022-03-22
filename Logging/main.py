import serial
import csv
import codecs
import time

#Serial port outputs all cell voltages contiguously (e.g. 300003100035000...29000\r\n)
#see image for clarification


serialPort = serial.Serial(port="COM3", baudrate=115200, bytesize=8, timeout=10, stopbits=serial.STOPBITS_ONE)
serialString = ""  # Used to hold data coming over UART
count = 0

with open('data.csv', 'w', newline='') as dataFile:
    dataWriter = csv.writer(dataFile)
    dataWriter.writerow(["both balance, time (ms)", "Cell 1", "Cell 2", "Cell 3", "Cell 4", "Cell 5", "Cell 6", "Cell 7", "Cell 8", "Cell 9",  "Cell 10", "Cell 11", "Cell 12", "Cell 13", "Cell 14", "Cell 15"])
    start = time.time()

while 1:
    if serialPort.in_waiting > 0:
        cells=[]
        cells=[0 for i in range(15)]
        #aaaa = serialPort.readline()
        #num1 = aaaa[0]

        for i in range(15):
            rawData = serialPort.read(5)
            cells[i] = rawData.decode('Ascii')

        endBits1 = serialPort.read(2)
        if(endBits1 == b'\r\n'):
            with open('data.csv', 'a', newline='') as dataFile:
                dataWriter = csv.writer(dataFile)
                dataWriter.writerow([str(count), str(cells[0]), str(cells[1]), str(cells[2]), str(cells[3]), str(cells[4]), str(cells[5]), str(cells[6]), str(cells[7]), str(cells[8]), str(cells[9]), str(cells[10]), str(cells[11]), str(cells[12]), str(cells[13]), str(cells[14])])
                print(count)
                if count > 10:
                    end = time.time()
                    print("Time:", end - start)
                    break

        count = count + 0.1
