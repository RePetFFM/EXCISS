import os
import sys
import commands
from datetime import datetime, timedelta

print len(sys.argv)

if(len(sys.argv) != 2):
    sys.exit(0)

times = int(sys.argv[1])

timeStr = commands.getoutput("sudo python ~/EXCISS/exciss_python_master/python/sendSerialCmd.py -s /dev/ttyAMA0 -b 9600 -c rdt | tail -2")
timeStr = timeStr.strip();
print timeStr
utc_dt = datetime.strptime(timeStr, '%Y-%m-%dT%H:%M:%S')+timedelta(days=times)
print utc_dt
out = commands.getoutput("sudo date -s "+timeStr)
