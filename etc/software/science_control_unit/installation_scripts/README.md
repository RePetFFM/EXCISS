# Prepare the Raspberry Pi SD card for flight

This document covers how to prepare and install SD cards for the EXCISS mission.

## Choosing and preparing the SC card.

For the mission we used SD cards from SanDisk: SanDisk Extreme xx GB

In principle also other brands, sizes and speeds should work. We sucessfully tested these
SD card in several projects and they never failed, so they seemed like a good choice.

### Verify the integrity of the card

**Before** installing the operating system, make sure that the SD card is fully functional. There are 
multiple programs which fill the card with data and read it back. This way faulty cards and corrupt sectors
can be spotted. Depending on the operating system these programs have shown to be useful:
* [h2testw](https://www.heise.de/download/product/h2testw-50539) - Graphical tool for Windows
* [f3](http://oss.digirati.com.br/f3/) - command line tool for Linux and Mac

When at least one of these tools confirms that the card is ok, proceed with the installation of the operating system:

### Install the operating system
Currently (February 2017) we use the **Raspbian Stretch lite** version of November 2017:

[Raspbian](https://www.raspberrypi.org/downloads/raspbian/) - Official download site

[Raspbian Strech lite November 2017](http://downloads.raspberrypi.org/raspbian_lite/images/raspbian_lite-2017-12-01/2017-11-29-raspbian-stretch-lite.zip) 

There are several ways to "burn" the image to the SD card. The Application [Etcher](https://etcher.io/) has
the nice feature of verifying the image after it has been written to the SD card.

### Headless WiFi preparation
In the boot drive of the SD-Card:

* create file: "wpa_supplicant.conf" with follwing content:
```
country=DE
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
network={
    ssid="SSID"
    psk="password"
    key_mgmt=WPA-PSK
}
```
(change country=## accordingly to match WiFi frequencies)

* create empty file: "ssh"  to enable SSH

[Detailed manual](https://core-electronics.com.au/tutorials/raspberry-pi-zerow-headless-wifi-setup.html) Note that the wpa_supplicant.conf is not complete in this manual.

### First time boot

Steps to execute with sudo raspi-config

### Prepare for mission

Download and execute the xyz script from this repository to make your Raspi space-ready.

