#!/usr/bin/env python3
# encoding: utf-8

import csv
import matplotlib
import os
import pprint
import re
import sys
import math


import numpy as np
from matplotlib import pyplot
from matplotlib.backends.backend_pdf import PdfPages
from collections import defaultdict

def draw_graph(title, file, order, data, error):
    # Save Graphs on PDF
    f_ = os.path.join('graphs', file)
    p = PdfPages(f_)

    arr = np.asarray

    x = arr([1, 2, 3, 4, 5, 6, 7, 8])
    y = np.array(list(data[order].values()), dtype=float)
    error_ = np.array(list((error[order]).values()), dtype=float)


    # Plot data
    #for thread in data[order]:
     #   print ('Plotting order ' + order + ' threads ' + str(thread) + ' value ' + str(data[order][thread]) + ' error ' + str(error[order][thread]))
      #  pyplot.errorbar(thread, data[order][thread], yerr=error[order][thread])

    print('error:')
    for num in y:
        print (str(num))

    linestyle = {"linestyle":"-", "linewidth":1, "markeredgewidth":2, "elinewidth":1, "capsize":3}    

    (a, b, c) = pyplot.errorbar(x, y, error_, color='b', **linestyle)
    pyplot.legend(loc=0)

    pyplot.xlim(0, 9)

    # Set title
    pyplot.title(title)
    pyplot.xlabel('Numero de Worker Threads')
    pyplot.ylabel('Tempo (segundos)')

    pyplot.savefig(p, format='pdf')
    pyplot.close()
    p.close()
    # pyplot.show()


def main():
    # Generate regex for file names
    p = re.compile('(?P<type>(par|seq))_matriz(?P<order>[0-9]*)(_(?P<threads>([0-9])))?\.out')

    data = defaultdict(dict)
    error = defaultdict(dict)
    

    for f in os.listdir('output'):
        # Get information from filename regex match
        m = p.match(f)
        if not m:
            print('invalid filename: ' + f + ', skipping...', sys.stderr)
            continue
        
        thread = 1 if (m.group('type') == 'seq') else int(m.group('threads'))

        order = m.group('order')

        f_ = os.path.join('output', f)
        
        sum_ = 0.0
        sum_error = 0.0
        with open(f_) as f:
            floats = []
            for line in f:
                float_line = line.split()
                for num in float_line:
                    floats.append(float(num))

            print ('Order: ' + order + ' Threads: ' + str(thread))

            total = 0
            for num in floats:
                print(str(num))
                sum_ += num
                total += 1
            print('sum: ' + str(sum_))
            sum_ /= total

            print ('Order: ' + order + ' Threads: ' + str(thread))
            
            for num in floats:
                sum_error += (num - sum_) * (num - sum_)
            sum_error /= total
            sum_error = math.sqrt(sum_error)

        data[order][thread] = sum_
        error[order][thread] = sum_error

    for order in data:
        draw_graph('Grafico de tempo por numero de threads para matriz de tamanho ' + order,'graph_' + order + '.pdf', order, data, error)


    return

      

if __name__ == "__main__":
    main()