#!/bin/sh
#set -e -u
# main mission simulation
# https://github.com/RePetFFM/EXCISS_DOKU/blob/master/EXCISS_mission_timeline.txt


echo "Set watchdog GPIO to high for 3 sec and keep it for 3min low. The arduino should try to gracefull shutdown the raspberry, if the watchdog signal is missing for predefined timespan."
echo "get date time from arduino via serial read request"
# TODO sudo python /home/pi/EXCISS/exciss_python_master/python/readSerialCmd.py -s /dev/serial0 -b 9600

echo "Read from arduino last stored status and error informations for health logfile."
# TODO

echo "take dark still image for pixel error reference"
python /home/pi/EXCISS/exciss_python_master/python/mission_cam_cmd.py -c "raspistill -md 4 -w 1640 -h 1232 -awb auto -v -o DATE_TIME_40fps_seg%02d.jpg 2>&1"

echo "turn on all LED\'s"
# TODO

echo "take 3 still images in 2 sec interval on USB Stick [main recording mass storage]"
python /home/pi/EXCISS/exciss_python_master/python/mission_cam_cmd.py -c "raspistill -md 4 -w 1640 -h 1232 -awb auto -v -o DATE_TIME_40fps_seg%02d.jpg 2>&1"
sleep 2s
python /home/pi/EXCISS/exciss_python_master/python/mission_cam_cmd.py -c "raspistill -md 4 -w 1640 -h 1232 -awb auto -v -o DATE_TIME_40fps_seg%02d.jpg 2>&1"
sleep 2s
python /home/pi/EXCISS/exciss_python_master/python/mission_cam_cmd.py -c "raspistill -md 4 -w 1640 -h 1232 -awb auto -v -o DATE_TIME_40fps_seg%02d.jpg 2>&1"


echo "start video recording with high framerate on USB Stick [main recording mass storage]"
sleep 2s
echo "start ignition"
# TODO start ignition

# TODO adjust for 3 minutes in script call. -t 180000
python /home/pi/EXCISS/exciss_python_master/python/mission_cam_cmd.py -c "raspivid -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 1000 -t 1800 -v -o DATE_TIME_40fps_seg%02d.h264 2>&1"

echo "stop high frame rate recording"

echo "start copying in background high frame rate video from USB Stick [main recording mass storage] to transfer USB stick"
DATETIME=date +"%d.%m.%y_%h.%m"
DIRECTORY="/home/pi/TRANSFER"+$DATETIME
if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi
mv /home/pi/EXCISS/exciss_python_master/python/*.h264 $DIRECTORY/
mv /home/pi/EXCISS/exciss_python_master/python/*.txt $DIRECTORY/
mv /home/pi/EXCISS/exciss_python_master/python/*.jpg $DIRECTORY/
echo "start copying in background high frame rate video from USB Stick [main recording mass storage] to backup USB stick"
echo "start low frame rate recording / begin still timelaps with burst LED exposure on USB Stick [main recording mass storage]"
echo "wait for 7 min"
# TODO correct time
sleep 7s #7m

echo "turn off all LED's"
echo "start copying low frame rate video from USB Stick [main recording mass storage] to backup USB stick"

echo "Write health logfile to Transfer USB"
# TODO

echo "unmount USB sticks"
# TODO ???
sudo umount /dev/sdb1
sudo umount /dev/sdb2

echo "send shutdown signal to Arduino via serial"
# TODO
sudo python /home/pi/EXCISS/exciss_python_master/python/sendSerialCmd.py -s /dev/serial0 -b 9600 -c "TODO_TURNOFF"

echo "Shutdown Raspberry Pi"
#sudo shutdown now
