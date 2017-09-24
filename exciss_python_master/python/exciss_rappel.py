#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Python script to handle the shaker (rappel wackel).
# Takes a waveform, e.g. 64,64,64,64,14,14,0 and the sleep time as parameters.
# 
# run:
# sudo python exciss_rappel.py -w "64,64,64,64,14,14,0" -s 3
#

import smbus
import time
import sys, getopt
import logging
import os

#########################LOGGING##########################
# log file name is identical to the script name
# ensure the log folder exists
logDirectory = '/home/pi/log'
if not os.path.exists(logDirectory):
  os.makedirs(logDirectory)
log_fileHandler = logging.FileHandler('/home/pi/log/'+ 'exciss_rappel' +'.log')

# logging
logger = logging.getLogger(str(sys.argv[0]))
logger.setLevel(logging.DEBUG)
log_fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
log_fileHandler.setFormatter(formatter)
logger.addHandler(log_fileHandler)
#########################LOGGING##########################

#########################WACKEL###########################
wackel = smbus.SMBus(1)

# constants
DRV2605_ADDR = 0x5A

DRV2605_REG_STATUS = 0x00
DRV2605_REG_MODE = 0x01
DRV2605_MODE_INTTRIG = 0x00
DRV2605_MODE_EXTTRIGEDGE = 0x01
DRV2605_MODE_EXTTRIGLVL = 0x02
DRV2605_MODE_PWMANALOG = 0x03
DRV2605_MODE_AUDIOVIBE = 0x04
DRV2605_MODE_REALTIME = 0x05
DRV2605_MODE_DIAGNOS = 0x06
DRV2605_MODE_AUTOCAL = 0x07

DRV2605_REG_RTPIN = 0x02
DRV2605_REG_LIBRARY = 0x03
DRV2605_REG_WAVESEQ1 = 0x04
DRV2605_REG_WAVESEQ2 = 0x05
DRV2605_REG_WAVESEQ3 = 0x06
DRV2605_REG_WAVESEQ4 = 0x07
DRV2605_REG_WAVESEQ5 = 0x08
DRV2605_REG_WAVESEQ6 = 0x09
DRV2605_REG_WAVESEQ7 = 0x0A
DRV2605_REG_WAVESEQ8 = 0x0B

DRV2605_REG_GO = 0x0C
DRV2605_REG_OVERDRIVE = 0x0D
DRV2605_REG_SUSTAINPOS = 0x0E
DRV2605_REG_SUSTAINNEG = 0x0F
DRV2605_REG_BREAK = 0x10
DRV2605_REG_AUDIOCTRL = 0x11
DRV2605_REG_AUDIOLVL = 0x12
DRV2605_REG_AUDIOMAX = 0x13
DRV2605_REG_RATEDV = 0x16
DRV2605_REG_CLAMPV = 0x17
DRV2605_REG_AUTOCALCOMP = 0x18
DRV2605_REG_AUTOCALEMP = 0x19
DRV2605_REG_FEEDBACK = 0x1A
DRV2605_REG_CONTROL1 = 0x1B
DRV2605_REG_CONTROL2 = 0x1C
DRV2605_REG_CONTROL3 = 0x1D
DRV2605_REG_CONTROL4 = 0x1E
DRV2605_REG_VBAT = 0x21
DRV2605_REG_LRARESON = 0x22

# Configuration of wackel
wackel = smbus.SMBus(1)

def writeRegister8(reg, val):
  wackel.write_byte_data(DRV2605_ADDR,reg,val)

def readRegister8(reg):
  return(wackel.read_byte_data(DRV2605_ADDR,reg))

def begin():
  id = readRegister8(DRV2605_REG_STATUS)
  writeRegister8(DRV2605_REG_MODE, 0x00)
  writeRegister8(DRV2605_REG_RTPIN, 0x00)
  writeRegister8(DRV2605_REG_WAVESEQ1, 1)
  writeRegister8(DRV2605_REG_WAVESEQ2, 0)
  writeRegister8(DRV2605_REG_OVERDRIVE, 0)
  writeRegister8(DRV2605_REG_SUSTAINPOS, 0)
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0)
  writeRegister8(DRV2605_REG_BREAK, 0)
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64)
  # ERM open loop

  # turn off N_ERM_LRA
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F)
  # turn on ERM_OPEN_LOOP
  writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20)

def setWaveform(slot, w):
  writeRegister8(DRV2605_REG_WAVESEQ1 + slot, w)

def selectLibrary(lib):
  writeRegister8(DRV2605_REG_LIBRARY, lib)

def go():
  writeRegister8(DRV2605_REG_GO, 1)

def stop():
  writeRegister8(DRV2605_REG_GO, 0)

def setMode(mode):
  writeRegister8(DRV2605_REG_MODE, mode)

def setRealtimeValue(rtp):
  writeRegister8(DRV2605_REG_RTPIN, rtp)

def useERM():
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F)

def useLRA():
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) | 0x80)

# Stuff from Christine here
def init():
  begin()
  setMode(DRV2605_MODE_INTTRIG)
  selectLibrary(1)
  useERM()

def set_effect_waveform(forms):
  cnt = 0
  for elm in forms:
    #print("id: " + str(cnt) + " = " + str(elm))
    setWaveform(cnt, elm)
    cnt += 1
    if cnt >= 8:
      break
  for idx in range(cnt,8):
    #print("idx " + str(idx))
    setWaveform(idx, 0)
#########################WACKEL###########################

#########################MAIN#############################
def main(argv):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)

  waveform = [64,64,64,64,14,14,0]
  sleepTime = 3
  try:
    logger.info('Reading command line parameters')
    opts, args = getopt.getopt(argv,"hw:s:",["waveform=", "sleep="])
  except getopt.GetoptError:
    print 'python ' + str(sys.argv[0]) + ' -w <waveform> -s <sleep>'
    logger.error('python ' + str(sys.argv[0]) + ' -w <waveform> -s <sleep>')
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print str(sys.argv[0]) + ' -w <waveform> -s <sleep>'
      sys.exit()
    elif opt in ("-w", "--waveform"):
      waveform = str(arg)
      waveform = waveform.split(',')
      index=0
      for i in waveform:
        waveform[index] = int(i)
        index += 1
    elif opt in ("-s", "--sleep"):
      sleepTime = int(arg)

  logger.info('Initialize rappel system')
  init()

  logger.info('Set waveforms (up to 8): ' + str(waveform))
  set_effect_waveform(waveform)

  logger.info('Start shaking')
  go()
  logger.info('Wait ' + str(sleepTime) + ' seconds')
  time.sleep(sleepTime)
  logger.info('Stop shaking')
  stop()

if __name__ == "__main__":
  main(sys.argv[1:])
#########################MAIN##########################
