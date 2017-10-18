#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# template for python modules
# python modules can be called from shell script with parameters
# python script logs to separate log file
#
# Author: Solveigh
#
# run:
# python moduleTemplate_readConfiguration.py -p 1
# python moduleTemplate_readConfiguration.py -p 1 -q 2

#
# https://docs.python.org/3/library/configparser.html#configparser.ConfigParser.get
# https://docs.python.org/2/howto/logging.html
# https://docs.python.org/2/library/getopt.html

import sys, getopt
import logging
import os

#########################LOGGING##########################
# log file name is identical to the script name
# ensure the log folder exists
logDirectory = '/home/pi/log'
if not os.path.exists(logDirectory):
  os.makedirs(logDirectory)
log_fileHandler = logging.FileHandler('/home/pi/log/cpu_temperature.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################


def readFile(fileName):
  tmp=0
  try:
    target = open(fileName, 'r')
    tmp = target.read()
    target.close()
    print('Read cpu temp in ' + fileName + ': ' + str(tmp))
  except OSError: # py2: FileNotFoundError:
    pass # ???
  return tmp

#########################MAIN##########################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  parameter1 = 0
  parameter2 = 0
  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"hp:q:",["parameter1=", "parameter2="])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0]) + ' -p <parameter1> -q <parameter2>'
    logger.error('python ' + str(sys.argv[0]) + ' -p <parameter1> -q <parameter2>')
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0]) + ' -p <parameter1> -q <parameter2>'
      sys.exit()
    elif opt in ("-p", "--parameter1"):
      parameter1 = int(arg)
    elif opt in ("-q", "--parameter2"):
      parameter2 = int(arg)

  print 'Parameter 1 is: ' + str(parameter1)
  logger.info('Parameter 1 is: ' + str(parameter1))
  print 'Parameter 2 is: ' + str(parameter2)
  logger.info('Parameter 2 is: ' + str(parameter2))

  tmp = readFile('/sys/class/thermal/thermal_zone0/temp')
  cputmp = float(tmp)/1000.0

  print('CPU temperature: ' + str(cputmp) + ' C')
  logger.info('CPU temperature: ' + str(cputmp) + ' C')

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
