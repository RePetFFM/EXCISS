#!/usr/bin/python
#
# Author: Oros
#
# required: you have to disable the internet time sync of the os
#           systemctl disable systemd-timesyncd
#           systemctl stop systemd-timesyncd
# description:
#  The parameter is the day multiplier to shift the time from 2000-01-00T00:00:00
#
# run: sudo ./setRtc.py 5

import os
import sys
import commands
from datetime import datetime, timedelta

if(len(sys.argv) != 2):
    print "you have to submit a number that represents the multiplier for the day shift!"
    sys.exit(0)

times = int(sys.argv[1])

timeStr = commands.getoutput("sudo python /home/pi/EXCISS/exciss_python_master/python/sendSerialCmd.py -s /dev/ttyAMA0 -b 9600 -c rdt | tail -2")
timeStr = timeStr.strip();
print timeStr
utc_dt = datetime.strptime("2000-1-1T0:0:0", '%Y-%m-%dT%H:%M:%S')
try:
    utc_dt = datetime.strptime(timeStr, '%Y-%m-%dT%H:%M:%S')
except ValueError:
    print "der Arduino hat nur Schrott geliefert!"

if(utc_dt.strftime ("%Y-%m-%dT%H:%M:%S") == "2000-01-01T00:00:00"):
    utc_dt = utc_dt+timedelta(days=times)
print utc_dt
out = commands.getoutput("sudo date -s "+utc_dt.strftime ("%Y-%m-%dT%H:%M:%S"))
