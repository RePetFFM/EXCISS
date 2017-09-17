#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# sendSerialCmd python module
# python modules can be called from shell script with parameters
# python script logs to separate log file
#
# Author: Solveigh
#
# required: sudo pip install pyserial
# https://raspberrypi.stackexchange.com/questions/45570/how-do-i-make-serial-work-on-the-raspberry-pi3
#
# run:
# sudo sendSerialCmd.py -p theSerialCommand

import sys, getopt
import logging
import time
import serial
import os

#########################LOGGING##########################
# log file name is identical to the script name
# ensure the log folder exists
logDirectory = '/home/pi/log'
if not os.path.exists(logDirectory):
  os.makedirs(logDirectory)
log_fileHandler = logging.FileHandler('/home/pi/log/sendSerial.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################

parameter1 = 0

# configure the serial connections (the parameters differ on the device you are connecting to)
ser = serial.Serial(
    port='/dev/serial0',
    baudrate=9600
)

def sendSerialCommand():
  global parameter1

  try:
    logger.info("Start serial communication.")

    ser.close() # close port in case it is still open

    ser.open()

    if ser.isOpen():
      logger.info("Serial port open.")
    else:
      logger.error("Serial port not open")

    logger.info("Serial port: " + ser.name)

    # TODO implement the serial communication here

    # send the character to the device
    # (note that I happend a \r\n carriage return and line feed to the characters - this is requested by my device)
    logger.info("Sending to serial port: " + str(parameter1) + '\r\n')
    ser.write(str(parameter1) + '\r\n')

    out = ''
    # let's wait one second before reading output (let's give device time to answer)
    time.sleep(1)
    while ser.inWaiting() > 0:
      out += str(ser.read(1))

      if out != '':
        print "received: " + out

  except serial.SerialException, e:
    logger.error("An exception occured: " + str(e))
  finally:
    logger.info("Closing serial port.")
    ser.close()

  logger.info("End serial communication.")

#########################MAIN##########################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  global parameter1
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

  sendSerialCommand()

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################