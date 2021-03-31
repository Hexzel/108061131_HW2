import matplotlib.pyplot as plt
import numpy as np
import time
import serial

board = '/dev/ttyACM0'
s = serial.Serial(board)

frequency = float(s.readline())
period = 1 / frequency;
x = np.arange(0, period * 10, period / 50)
n = len(x);
y = np.arange(0, n, 1.0)
for i in range(0, n):
	line = s.readline()
	y[i] = float(line)
	#print([i, line, y[i]])

print('receive successfully')
y_no_mean = y
y = y - np.mean(y)
frq = np.arange(int(n/2)) * (frequency / 20.0)
Y = np.fft.fft(y)/n*2
Y = Y[range(int(n/2))]

fig, ax = plt.subplots(2,1)
ax[0].plot(x, y_no_mean)
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Amplitude')
ax[1].plot(frq[range(40)], abs(Y[range(40)]))
ax[1].set_xlabel('Freq (Hz)')
ax[1].set_ylabel('|Y(freq)|')
plt.show()
s.close()
