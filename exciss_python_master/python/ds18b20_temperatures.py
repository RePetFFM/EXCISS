#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# python modules to read temperatures from a chain of ds18b20 sensors wired via i2c
# python script logs to separate log file
#
# Author: Solveigh
#
# run:
# python moduleTemplate_ds18b20_temperatures.py

#
# https://docs.python.org/3/library/configparser.html#configparser.ConfigParser.get
# https://docs.python.org/2/howto/logging.html
# https://docs.python.org/2/library/getopt.html
# https://learn.adafruit.com/adafruits-raspberry-pi-lesson-11-ds18b20-temperature-sensing/ds18b20
# https://github.com/timofurrer/w1thermsensor

import sys, getopt
import logging
import os
import datetime

from w1thermsensor import W1ThermSensor

CSV_FILENAME = 'log/temperatures.csv'

#########################LOGGING##########################
# log file name is identical to the script name
# ensure the log folder exists
logDirectory = '/home/pi/log'
if not os.path.exists(logDirectory):
  os.makedirs(logDirectory)
log_fileHandler = logging.FileHandler('/home/pi/log/'+ 'ds18b20_temperatures' +'.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################

#########################FILE#############################
def writeCSVFile(fileName, sensor_id, temperature):
  print('Store temperatures in ' + fileName + ': ' + str(sensor_id) + ': ' + str(temperature))
  logger.info('Store temperatures in ' + fileName + ': ' + str(sensor_id) + ': ' + str(temperature))
  now = datetime.datetime.now()
  dateAndTime = str(now.day) + '.' + str(now.month) + '.' + str(now.year) + ',' + str(now.strftime('%H')) + ':' + str(now.strftime('%M'))
  target = open(fileName, 'a')
  target.write(dateAndTime + ',' + str(sensor_id ) + ',' + str(temperature) + '\r\n')
  target.close()
  return True
######################END##FILE###########################

#########################MAIN#############################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"h",[])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0])
    logger.error('python ' + str(sys.argv[0]))
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0])
      sys.exit()

  try:
    exciss_home = str(os.environ['EXCISS_HOME'])
    logger.info('Environment variable EXCISS_HOME=' + exciss_home)
    print('Environment variable EXCISS_HOME=' + exciss_home)
  except KeyError:
    logger.error('Environment variable EXCISS_HOME not set.')
    print('Environment variable EXCISS_HOME not set.')
    exit()

  # collect temperatures from all the connected sensors
  for sensor in W1ThermSensor.get_available_sensors():
    print('Sensor %s has temperature %.2f' % (sensor.id, sensor.get_temperature()))

    # log temperatures to csv file
    writeCSVFile(exciss_home + '/' + CSV_FILENAME, sensor.id, sensor.get_temperature())

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
