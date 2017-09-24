#!/bin/bash
#set -e -u
# main mission simulation
# https://github.com/RePetFFM/EXCISS_DOKU/blob/master/EXCISS_mission_timeline.txt

# skip ignition:
# ./exciss_mission_main.sh 

#########################CONFIG################################
EXCISS_HOME="/home/pi/EXCISS"
PYTHON_SCRIPTS="${EXCISS_HOME}/exciss_python_master/python"
SERIAL_NAME="/dev/ttyAMA0"
SERIAL_BAUD_RATE="9600"

RASPI_CAM_STILL_IMAGE_1="raspistill -md 4 -w 1640 -h 1232 -awb auto -v -e png -o DATE_TIME_calibImgLedsOn_1.png 2>&1"
RASPI_CAM_STILL_IMAGE_2="raspistill -md 4 -w 1640 -h 1232 -awb auto -v -e png -o DATE_TIME_calibImgLedsOn_2.png 2>&1"
RASPI_CAM_STILL_IMAGE_3="raspistill -md 4 -w 1640 -h 1232 -awb auto -v -e png -o DATE_TIME_calibImgLedsOn_3.png 2>&1"
RASPI_CAM_DARK_STILL_IMAGE="raspistill -md 4 -w 1640 -h 1232 -awb auto -v -e png -o DATE_TIME_calibImgLedsOff.png 2>&1"
#RASPI_CAM_VIDEO="raspivid -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 10000 -t 180000 -v -o DATE_TIME_3min_40fps_seg%02d.h264 2>&1"
# TODO short test call
RASPI_CAM_VIDEO="raspivid -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 1000 -t 5000 -v -o DATE_TIME_5s_40fps_seg%02d.h264 2>&1"

#########################END#CONFIG############################

cd /home/pi/mission/data

echo "PYTHON_SCRIPTS=${PYTHON_SCRIPTS}"

# optional parameter to skip ignition
SKIP_IGNITION="false"
if [ "$1" = "skipIgnition" ]; then
  echo "Skip ignition parameter available: $1"
  $SKIP_IGNITION = "true"
fi

#
# Set watchdog GPIO
#
echo "Set watchdog GPIO to high for 3 sec and keep it for 3min low. The arduino should try to gracefull shutdown the raspberry, if the watchdog signal is missing for predefined timespan."
echo "get date time from arduino via serial read request"
# TODO sudo python ${PYTHON_SCRIPTS}/readSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE}

#
# Read Arduino last status
#
echo "Read from arduino last stored status and error informations for health logfile."
# TODO

#
# Still image LEDs off
#LED front off:
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c wf0
#LED back off:
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c wb0

echo "take dark still image for pixel error reference"
python ${PYTHON_SCRIPTS}/mission_cam_cmd.py -c ${RASPI_CAM_DARK_STILL_IMAGE}

#
# Turn on LEDs
#
echo "turn on all LEDs"
#LED front full power:
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c wf255

#LED back full power:
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c wb255



#
# Still images LEDs on
#
echo "take 3 still images in 2 sec interval on USB Stick [main recording mass storage]"
# Reminder: there is a 5 second  white balancing action before taking the picture
python ${PYTHON_SCRIPTS}/mission_cam_cmd.py -c ${RASPI_CAM_STILL_IMAGE_1}
sleep 2s
python ${PYTHON_SCRIPTS}/mission_cam_cmd.py -c ${RASPI_CAM_STILL_IMAGE_2}
sleep 2s
python ${PYTHON_SCRIPTS}/mission_cam_cmd.py -c ${RASPI_CAM_STILL_IMAGE_3}


echo "start video recording with high framerate on microSD"
python ${PYTHON_SCRIPTS}/mission_cam_cmd.py -c ${RASPI_CAM_VIDEO} &
echo $!
ps -ef  | grep $!

sleep 2s

echo "skip? $SKIP_IGNITION"
if [ "$SKIP_IGNITION" == "false" ] ; then
  echo "start ignition"
  # TODO start ignition

  # TODO adjust for 3 minutes in script call. -t 180000
  #sleep 10s
fi

echo "high frame rate recording stopped"

#LED front off:
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c wf0

#LED back off:
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c wb0



exit 999

echo "start copying in background high frame rate video from USB Stick [main recording mass storage] to transfer USB stick"
DATETIME=date +"%d.%m.%y_%h.%m"
DIRECTORY="/home/pi/TRANSFER"+$DATETIME
if ! -d "$DIRECTORY" ; then
  mkdir $DIRECTORY
fi
mv ${PYTHON_SCRIPTS}/*.h264 $DIRECTORY/
mv ${PYTHON_SCRIPTS}/*.txt $DIRECTORY/
mv ${PYTHON_SCRIPTS}/*.jpg $DIRECTORY/
echo "start copying in background high frame rate video from USB Stick [main recording mass storage] to backup USB stick"
sudo mount /dev/sdb1
mv $DIRECTORY/* /dev/sdb1

echo "start low frame rate recording / begin still timelaps with burst LED exposure on USB Stick [main recording mass storage]"
# TODO
echo "wait for 7 min"
# TODO correct time
sleep 7s #7m

echo "turn off all LED's"
echo "start copying low frame rate video from USB Stick [main recording mass storage] to backup USB stick"
# TODO
echo "Write health logfile to Transfer USB"
# TODO

# TODO preserve log files of various python scripts?
# check directory /home/pi/log

echo "unmount USB sticks"
# TODO ???
sudo umount /dev/sdb1
sudo umount /dev/sdb2

echo "send shutdown signal to Arduino via serial"
# TODO
sudo python ${PYTHON_SCRIPTS}/sendSerialCmd.py -s ${SERIAL_NAME} -b ${SERIAL_BAUD_RATE} -c "TODO_TURNOFF"

echo "Shutdown Raspberry Pi"
#sudo shutdown now
