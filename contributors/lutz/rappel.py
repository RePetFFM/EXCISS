#!/usr/bin/python
import smbus
import time

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

# init rappel system
init()

# set waveforms (up to 8)
set_effect_waveform([64,64,64,64,14,14,0])

go()           # start shaking
time.sleep(3)  # wait 3 seconds
stop()         # stop shaking

