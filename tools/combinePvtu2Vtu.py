import vtk

import sys
import os
import multiprocessing
import functools

"""Combines all vtu files referenced by one pvtu to one single vtu.
This script processes up to N pvtu files in parallel, where N is the number of logical cores on the system.
"""

convertedPrefix = 'combined_'

################### VTU ####################

def convertVtu(vtufilename, appender, deleteInput):

    reader = vtk.vtkXMLUnstructuredGridReader()
    reader.SetFileName(vtufilename)
    reader.Update()
    readerOutput = reader.GetOutput()

    appender.AddInputData(readerOutput)
    if deleteInput:
        os.remove(vtufilename)

################### PVTU ###################

def convertPVtu(pvtuFilename, outputDir, deleteInput):
    pvtuDir = os.path.dirname(pvtuFilename)
    outputFile = os.path.join(outputDir, convertedPrefix + os.path.splitext(os.path.basename(pvtuFilename))[0] + '.vtu')
    print(pvtuFilename + ' -> ' + outputFile)
    # This thing can collect data from multiple datasets
    appender = vtk.vtkAppendFilter()

    with open(pvtuFilename, 'r') as pvtuFileIn:
        for line in pvtuFileIn:
            if '<Piece Source' in line:
                # extract sub vtu filenames
                vtuFilename = line.split('"')[1]
                convertVtu(os.path.join(pvtuDir, vtuFilename), appender, deleteInput)

    appender.Update()
    writer = vtk.vtkXMLUnstructuredGridWriter()
    writer.SetFileName(outputFile)
    appenderOutput = appender.GetOutput()
    writer.SetInputData(appenderOutput)
    # Create the output dir if it doesn't exist yet
    os.makedirs(outputDir, exist_ok=True)
    writer.Write()
    if deleteInput:
        os.remove(pvtuFilename)

############################################ Script ############################################
if __name__ == '__main__':
    # Require at least two arguments.
    if len(sys.argv) < 3:
         raise RuntimeError(f"""\
 Not enough input arguments.

 Usage: {os.path.basename(sys.argv[0])} [--remove] outputDir files.pvtu...
 
 --remove       If set, remove all input vtu and pvtu files.
 """)

    optArgs = 0
    remove = False
    if sys.argv[1] == '--remove':
        optArgs += 1
        remove = True

    outputDir = os.path.normpath(sys.argv[optArgs + 1])
    files = sys.argv[optArgs + 2:]
    # bind the output dir argument to the function
    convertPVtuWrapped = functools.partial(convertPVtu, outputDir=outputDir, deleteInput=remove)
    # parallelize over the file list
    multiprocessing.Pool().map(convertPVtuWrapped, files)
