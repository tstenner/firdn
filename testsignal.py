import numpy as np
import matplotlib.pyplot as plt
import subprocess
import sys

fs = 500
duration = 10
freqs = [0, 1, 8, 60, 220, 400]
nchan = len(freqs)
t = np.linspace(0, duration, duration*fs)
signal = np.zeros((nchan, duration*fs))
for i, f in enumerate(freqs):
    signal[i,:] = np.sin(2*np.pi*t*f)
signal[0,:]+=.5
signal[1,[100, 500, 2000]] = 1
signal.T.tofile('testsignal')
subprocess.run(['build/downsample', 'testsignal', str(nchan), '2', 'filt_2x']) 

infile = np.fromfile('testsignal').reshape((-1,nchan)).T
outfile = np.fromfile('testsignal.out').reshape((-1,nchan)).T
t2 = np.linspace(0, duration, duration*fs/2)
for i in range(nchan):
    plt.plot(t2,outfile[i,:])
    plt.plot(t,infile[i,:])
    plt.show(block=True)
