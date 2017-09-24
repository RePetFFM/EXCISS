#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# python module to read the NASA ISS time file
#
# Author: Solveigh
#
# run:
# python evaluate_NASA_ISS_time.py -p "/home/solveigh/Desktop/exciss/EXCISS/contributors/solveigh/NASA_ISS_TIME_DUMMY_FILE"

import sys, getopt
import logging
import os

#########################LOGGING##########################
# log file name is identical to the script name
# ensure the log folder exists
logDirectory = '/home/pi/log'
if not os.path.exists(logDirectory):
  os.makedirs(logDirectory)
log_fileHandler = logging.FileHandler('/home/pi/log/'+ 'evaluate_NASA_ISS_time' +'.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################

#########################FILE#############################
def readFile(fileName):
  try:
    target = open(fileName, 'r')
    time_string = str(target.read())
    target.close()
    print(str(time_string))
    logger.info('Content of ' + fileName + ': ' + time_string)

    '''
    # split time and date string
    splitted_time = time_string.split('_')
    if len(splitted_time) == 6:
      print('Y ' + splitted_time[0])
      print('M ' + splitted_time[1])
      print('D ' + splitted_time[2])
      print('h ' + splitted_time[3])
      print('m ' + splitted_time[4])
      print('s ' + splitted_time[5])
    '''
    return time_string
  except OSError: # py2: FileNotFoundError:
    logger.error('Error reading file: ' + fileName)
    return ""
  return ""
#########################FILE#############################

#########################MAIN##########################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  parameter1 = 0
  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"hp:",["parameter1="])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0]) + ' -p <parameter1>'
    logger.error('python ' + str(sys.argv[0]) + ' -p <parameter1>')
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0]) + ' -p <parameter1>'
      sys.exit()
    elif opt in ("-p", "--parameter1"):
      parameter1 = str(arg)

  print 'Parameter 1 is: ' + str(parameter1)
  logger.info('Parameter 1 is: ' + str(parameter1))

  readFile(parameter1)

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
