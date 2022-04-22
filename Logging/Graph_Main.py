import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import random
import serial

# initialize serial port
serialPort = serial.Serial(port="COM3", baudrate=115200, bytesize=8, timeout=10, stopbits=serial.STOPBITS_ONE)
serialString = ""  # Used to hold data coming over UART
count = 0

# ser = serial.Serial()
# ser.port = '/dev/ttyACM0'  # Arduino serial port
# ser.baudrate = 9600
# ser.timeout = 10  # specify timeout when using readline()
# ser.open()
# if ser.is_open == True:
#     print("\nAll right, serial port now open. Configuration:\n")
#     print(ser, "\n")  # print serial parameters

# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []  # store trials here (n)
yData = [ []*1 for i in range(15)]
ys = []  # store relative frequency here
y1s =[]
y2s =[]
y3s =[]
y4s =[]
y5s =[]
i=0


# This function is called periodically from FuncAnimation
def animate(i, xs, ys):
    # Aquire and parse data from serial port
    i+=0.1
    cells = []
    cells = [0 for i in range(19)]
    for j in range(19):
        rawData = serialPort.read(5)
        cells[j] = rawData.decode('Ascii')
    endBits1 = serialPort.read(2)

    # Add x and y to lists
    xs.append(i)
    for ii in range(15):
        yData[ii].append(cells[ii])
    ax.clear()

    for jj in range(15):
        ax.plot(xs, yData[jj], label="Cell Voltage")
    plt.title('Cell Voltage')
    plt.ylabel('Voltage')
    plt.legend()


ani = animation.FuncAnimation(fig, animate, fargs=(xs, ys), interval=100)
plt.show()