

## PC 
copy hex and conf file to desktop in 'exciss' folder.

`-P[comport]` (windows key + r) -> devmgmt.msc -> Ports (COM & LPT)

if arduino is a 32bit version, remove `(x86)` 

```
"%ProgramFiles(x86)%\Arduino\hardware\tools\avr\bin\avrdude" -C"%userprofile%\desktop\exciss\avrdude.conf" -v -patmega2560 -cwiring -PCOM6 -b57600 -D -Uflash:w:"%userprofile%\desktop\exciss\exciss.ino.arduino_mega.hex":i 
```

## MAC
/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega2560 -cwiring -P/dev/cu.usbserial-A6XWR9BI -b57600 -D -Uflash:w:/var/folders/c4/qr7lhph91d3235cp0939xyd40000gp/T/arduino_build_377617/exciss.ino.hex:i 
