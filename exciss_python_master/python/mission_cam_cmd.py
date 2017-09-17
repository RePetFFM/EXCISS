#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Mainmission python module
# python modules can be called from shell script with parameters
# python script logs to separate log file
#
# Author: Solveigh, Rainer
#
#
# This Python file takes an command line to record pictures or videos with the PiCamera
#
# Example:
# raspivid -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 10000 -t 180000 -o 2017-09-17_20-29-12_40fps_seg%02d.h264
# The date and the time have to be filled by the Python script, so we use dummy variables in the string we send from earth:
# raspivid -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 10000 -t 180000 -o DATE_TIME_40fps_seg%02d.h264
#
# run:
# python mission_cam_cmd.py -h
# python mission_cam_cmd.py -c "raspivid ..."
#
#
# Command line parameter doku:
# https://www.raspberrypi.org/documentation/raspbian/applications/camera.md
#

import sys, getopt
import logging
import os
import datetime

#########################LOGGING##########################
# log file name is identical to the script name
# ensure the log folder exists
logDirectory = '/home/pi/log'
if not os.path.exists(logDirectory):
  os.makedirs(logDirectory)
log_fileHandler = logging.FileHandler('/home/pi/log/mission_cam_cmd.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################

cmd = 'raspivid -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 1000 -t 1000 -v -o DATE_TIME_40fps_seg%02d.h264 2>&1' # backup string if something goes wrong

def runTheMission():
  global cmd

  logger.info("Launch the mission.")

  try:
    # replace the DATE and TIME part in the filename with the actual date and time
    # find the date in the form 2017-09-17 (YYYY-mm-dd)
    now = datetime.datetime.now()
    strDate = str(now.strftime('%Y-%m-%d'))

    # find the time in the form 21-57-22 (HH-MM-SS)
    strTime = str(now.strftime('%H-%M-%S'))
    print "Date: " + strDate
    print "Time: " + strTime

    print "Command BEFORE replacement"
    print cmd

    cmd = cmd.replace("DATE", strDate)
    cmd = cmd.replace("TIME", strTime)

    print "Command AFTER replace"
    print cmd

    logger.info("Cmd: " + cmd)
    ret = os.popen(cmd).read()

    # in 'ret' we now have the output of the raspivid command. Now we save it to a file with the same timestamp than the first video segment
    # we use the date and time from the beginning of the experiment for this
    strOutputTxtFilename = strDate + "_" + strTime + "_CmdLineOutput.txt"
    myFile = open(strOutputTxtFilename, 'w')
    myFile.write(ret)
    myFile.close()

  except:
    logger.error("Exception in call of raspivid.")
  finally:
    logger.info("End the mission.")

#########################MAIN##########################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  global cmd

  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"hc:",["cmd="])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0]) + ' -c <cmd>'
    logger.error('python ' + str(sys.argv[0]) + ' -c <cmd>')
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0]) + ' -c <cmd>'
      sys.exit()
    elif opt in ("-c", "--cmd"):
      cmd = str(arg)

  logger.info('Parameter cmd is: ' + str(cmd))

  runTheMission()

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
