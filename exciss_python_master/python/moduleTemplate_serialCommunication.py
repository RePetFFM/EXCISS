#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Mainmission python module
# python modules can be called from shell script with parameters
# python script logs to separate log file
#
# Author: Solveigh
#
# run:
# sudo moduleTemplate_serialCommunication.py -p 1
# sudo moduleTemplate_serialCommunication.py -p 1 -q 2

#
# https://docs.python.org/3/library/configparser.html#configparser.ConfigParser.get
# https://docs.python.org/2/howto/logging.html
# https://docs.python.org/2/library/getopt.html

import sys, getopt
import logging
import time
import serial

#########################LOGGING##########################
# note: this folder has to exist
# log file name is identical to the script name
log_fileHandler = logging.FileHandler('/home/pi/log/'+ str(sys.argv[0]) +'.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################

parameter1 = 0
parameter2 = 0


# configure the serial connections (the parameters differ on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyS0',
    baudrate=9600,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
)

def doSerialCommunication():
  global parameter1
  global parameter2

  try:
    logger.info("Start serial communication.")
    ser.open()
    ser.isOpen()

    logger.info("Serial port: " + ser.name)

    # send the character to the device
    # (note that I happend a \r\n carriage return and line feed to the characters - this is requested by my device)
    ser.write('SOME_CODE' + '\r\n')

    out = ''
    # let's wait one second before reading output (let's give device time to answer)
    time.sleep(1)
    while ser.inWaiting() > 0:
      out += ser.read(1)

      if out != '':
        print ">> " + out

  except SerialException:
    logger.error("An exception occured.")
  finally:
    logger.info("Closing serial port.")
    ser.close()
  # TODO implement the serial communication here


  logger.info("End serial communication.")

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

  runTheMission()

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################