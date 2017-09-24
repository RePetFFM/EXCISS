#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Python module to read the NASA ISS time file
# and set or restore the time of the RTC via serial connection.
#
# Nothing happens if the file is not available
#
# Author: Solveigh
#
# run:
# python evaluate_NASA_ISS_time.py -f "/home/solveigh/Desktop/exciss/EXCISS/contributors/solveigh/NASA_ISS_TIME_DUMMY_FILE" -m "set/restore" -s "/dev/ttyAMA0" -b 9600

import sys, getopt
import logging
import os
import serial
import time

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

    return time_string
  except IOError:
    logger.error('Error reading file: ' + fileName)
    return ""
  except OSError:
    logger.error('Error reading file: ' + fileName)
    return ""
  return ""
#########################FILE#############################

#########################SERIAL###########################
serialPort = '/dev/ttyAMA0'
baudRate = 9600

def sendSerialCommand(cmd):
  global serialPort
  global baudRate

  ser = None

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
    if ser != None:
      ser.close()

  logger.info("End serial communication.")
#########################SERIAL###########################

#########################MAIN#############################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  fileName = 0
  mode = 0
  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"hf:m:s:b:",["filename=", "mode=", "serialPort=", "baudRate="])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0]) + ' -f <filename> -m <mode> -s <serialPort> -b <baudRate>'
    logger.error('python ' + str(sys.argv[0]) + ' -f <filename> -m <mode> -s <serialPort> -b <baudRate>')
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0]) + ' -f <filename> -m <mode> -s <serialPort> -b <baudRate>'
      sys.exit()
    elif opt in ("-f", "--filename"):
      filename = str(arg)
    elif opt in ("-m", "--mode"):
      mode = str(arg)
    elif opt in ("-s", "--serialPort"):
      serialPort = str(arg)
    elif opt in ("-b", "--baudRate"):
      baudRate = int(arg)

  print 'Filename is: ' + str(filename)
  logger.info('Filename is: ' + str(filename))
  print 'Mode is: ' + str(mode)
  logger.info('Mode is: ' + str(mode))
  
  time_string = readFile(filename)
  
  if time_string != "":
    if mode == "set":
      logger.info('Set time and date')
      sendSerialCommand("wtd" + time_string)
    if mode == "restore":
      logger.info('Restore time and date')
      sendSerialCommand("wtr" + time_string)
  else:
    print('Nothing done, no time and date file available: ' + time_string)
    logger.info('Nothing done, no time and date file available: ' + time_string)

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
