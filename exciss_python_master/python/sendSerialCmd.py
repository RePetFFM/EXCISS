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
# sudo sendSerialCmd.py -s /dev/serial0 -b 9600 -c theSerialCommand

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

serialPort = '/dev/serial0'
baudRate = 9600
cmd = 0

def sendSerialCommand():
  global serialPort
  global baudRate
  global cmd

  try:
    logger.info("Start serial communication.")

    # configure the serial connections (the parameters differ on the device you are connecting to)
    ser = serial.Serial(
      port=serialPort,
      baudrate=baudRate
    )

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
    logger.info("Sending to serial port: " + str(cmd) + '\r\n')
    ser.write(str(cmd) + '\r\n')

    out = ''
    # let's wait one second before reading output (let's give device time to answer)
    time.sleep(1)
    while ser.inWaiting() > 0:
      out += str(ser.read(1))
      if out != '':
        if '\r\n' in out:
          print out

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

  global serialPort
  global baudRate
  global cmd
  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"hs:b:c:",["serialPort=", "baudRate=", "cmd="])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0]) + ' -s <serialPort> -b <baudRate> -c <cmd>'
    logger.error('python ' + str(sys.argv[0]) + ' -s <serialPort> -b <baudRate> -c <cmd>')
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0]) + ' -s <serialPort> -b <baudRate> -c <cmd>'
      sys.exit()
    elif opt in ("-s", "--serialPort"):
      serialPort = str(arg)
    elif opt in ("-b", "--baudRate"):
      baudRate = int(arg)
    elif opt in ("-c", "--cmd"):
      cmd = str(arg)

  print 'Serial port is: ' + str(serialPort)
  logger.info('Serial port is: ' + str(serialPort))
  print 'Baudrate is: ' + str(baudRate)
  logger.info('Baudrate is: ' + str(baudRate))
  print 'Command is: ' + str(cmd)
  logger.info('Command is: ' + str(cmd))

  sendSerialCommand()

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
