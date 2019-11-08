import numpy as np
import scipy.signal as signal
import sys

if len(sys.argv) < 3:
    print(f"Usage: {sys.argv[0]} ntaps downrate")
    print("""

    Create FIR lowpass filters and export them as binary and C source files.
    """)

flts = []
for i in range(1, len(sys.argv), 2):
    ntaps = int(sys.argv[i])
    downrate = int(sys.argv[i+1])
    flts.append(signal.firwin(ntaps, .8 / downrate))
    flts[-1].tofile(f'filt_{downrate}x')
    arrayname = f'const double filt_{downrate}x[{ntaps}]'
    with open(f'filt_{downrate}x.c', 'xt') as f:
        f.write(f'static {arrayname} = {{')
        f.write(','.join((np.format_float_scientific(val) for val in flts[-1])))
        f.write('};\n')
    print(f'extern {arrayname};\n')

try:
    import matplotlib.pyplot as plt
    for flt in flts:
        w, h = signal.freqz(flt)
        plt.plot(w, 20*np.log10(np.abs(h)))
    plt.show(block=True)
except Exception:
    pass
