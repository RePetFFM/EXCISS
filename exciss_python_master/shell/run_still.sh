#!/bin/sh

# test script to launch the mission_cam python module
python /home/pi/EXCISS/exciss_python_master/python/mission_cam_cmd.py -c "raspistill -md 4 -w 1640 -h 1232 -fps 40 -ex fixedfps -awb auto --segment 1000 -v -o DATE_TIME_40fps_seg%02d.jpg 2>&1"
