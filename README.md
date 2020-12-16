# USB-WatchDog-firmware <a name="top"></a>

[![version](https://img.shields.io/badge/Version-0.1-BrightGreen)](https://github.com/MB6718/USB-WatchDog-firmware)
[![CXSTM8](https://img.shields.io/badge/compiler-CXSTM8-blue)](https://www.cosmicsoftware.com/download_stm8_free.php)
[![STVD-STM8](https://img.shields.io/badge/IDE-STVD&#x2010;STM8-blueviolet)](https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm8-software-development-tools/stm8-ides/stvd-stm8.html)
[![SPL-STM8](https://img.shields.io/badge/lib-SPL&#x2010;STM8-ff69b4)](https://www.st.com/en/embedded-software/stsw-stm8069.html)
[![license](https://img.shields.io/badge/license-GPL_v3.0-yellow)](https://github.com/MB6718/USB-WatchDog-firmware/blob/main/LICENSE)


### Content
* [Short description](#description)
* [Terms of reference and project concept](#concept)
* [Exchange protocol specification (requests / responses)](#specification)
	* [Null byte or Null command](#null_cmd)
    * [Command to set timeout](#time_limit_cmd)
    * [Hello command](#hello_cmd)
    * [Command for getting device version](#get_device_version_cmd)
    * [Command to switch to "soft reset" mode](#soft_reset_mode_cmd)
    * [Command to switch to "hard reset" mode](#hard_reset_mode_cmd)
    * [Command to switch to "power off" mode](#power_off_mode_cmd)
    * [Command "soft reset](#soft_reset_cmd)
    * [Command "hard reset](#hard_reset_cmd)
    * [Command "power off](#power_off_cmd)
* [Build with](#build_with)
* [Authors](#authors)
* [License](#license)


### Short description: <a name="description"></a>
This firmware is intended for use in widely used Chinese-made USB WatchDog devices.  
The standard firmware supplied with the device has a number of limitations and bugs.  
Firmware is designed to get rid of bugs and significantly expand the functionality of the device, while leaving backward compatibility with the original firmware.
> NOTE! Firmware only for devices based on STM8 micro-controller. As a development environment, the environment from ["ST Microelectronics - ST Visual Develop"](#build_with_1) with the ["Cosmic Software - Cosmic C compiler for STM8"](#build_with_2) was used. A set of libraries from ["ST Microelectronics - STM8S/A Standard peripheral library"](#build_with_3) is also used.


### Terms of reference and project concept: <a name="concept"></a>
The version 0.1 concept includes the following items:
1. Creating an acceptable (backward compatible) firmware for use with the Chinese utility.
2. Adding the ability to determine the firmware version.
3. Adding the ability to identify the author of the firmware.
4. Adding more than one operation mode.
5. Adding the function of non-volatile storage of the set timeout and operating mode.


### Exchange protocol specification (requests / responses): <a name="specification"></a>
The protocol used is one byte.

<a name="null_cmd"></a>
* **Null byte or Null command**

**Request code** &rArr; `0х00`:  
This command does nothing and does not affect the operation of the device in any way.  
**Response code** &rArr; `none`


<a name="time_limit_cmd"></a>
* **Command to set time limit**

**Request code** &rArr; `0х01 - 0х7F`:  
Command sets the timeout triggering device in the range from min to max time value. Time setting range from min 10 sec to max 1270 sec (~ 21 min 10 sec) in 10 sec increments. The command calculates as follows:  
`time / 10 = value in HEX`.  
Example: 20 sec = 0x02; 5 min 30 sec = 0x33; 19 min 50 sec = 0хА4.  
**Response code** &rArr; `0хAA` - Accept code


<a name="hello_cmd"></a>
* **Hello command**

**Request code** `0x80`:  
The command sends a hello message to the device. If there is no answer, then there is no device on the line. If the device returns code 0x81, the device is ready for operation. Sending this command will reset the device's internal time counter - this is the main function of this command.  
**Response code** `0x81` - Device ready


<a name="hello_cmd"></a>
* **Device check command**

**Request code** &rArr; `0x81`:  
The command sends a verification message to the device. If there is no answer, then there is a device from an unknown manufacturer on the line. If the device returns the code 0x80, the device is recognized and ready to work. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0х80` - Device ready


<a name="get_device_version_cmd"></a>
* **Command for getting device version**

**Request code** &rArr; `0x88`  
Command for requesting the device (firmware) version. In response to the request of this command, we get the value of the range 0x01 - 0x7F. To decipher the version, we use the following formula: received value / 10 = floating point result, where the comma acts as a version separator (the range of possibly identifiable versions is from 0.1 to 12.7). Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0x01 - 0x7F` - Device (firmware) version


<a name="soft_reset_mode_cmd"></a>
* **Command to switch to "soft reset" mode**

**Request code** &rArr; `0xA0`  
The command switches the device to the "soft reset" operating mode. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0хАА` - Accept code


<a name="soft_reset_mode_cmd"></a>
* **Command to switch to "hard reset" mode**

**Request code** &rArr; `0xA1`  
The command switches the device to the "hard reset" operating mode. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0хАА` - Accept code


<a name="soft_reset_mode_cmd"></a>
* **Command to switch to "power off" mode**

**Request code** &rArr; `0xA2`  
The command switches the device to the "power off" operating mode. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0хАА` - Accept code


<a name="soft_reset_cmd"></a>
* **Command "soft reset"**

**Request code** &rArr; `0хFF`  
Execute "soft reset" command. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0хАА` - Accept code


<a name="hard_reset_cmd"></a>
* **Command "hard reset"**

**Request code** &rArr; `0хFF`  
Execute "hard reset" command. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0хАА` - Accept code


<a name="power_off_cmd"></a>
* **Command "power off"**

**Request code** &rArr; `0хFF`  
Execute "power off" command. Sending this command will reset the device's internal clock.  
**Response code** &rArr; `0хАА` - Accept code


### Построен с использованием <a name="build_with"></a>

* [STVD-STM8](https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm8-software-development-tools/stm8-ides/stvd-stm8.html) - ST Visual develop IDE for developing ST7 and STM8 applications <a name="build_with_1"></a>
* [CXSTM8](https://www.cosmicsoftware.com/download_stm8_free.php) - COSMIC C Cross Compiler for ST Microelectronics STM8 <a name="build_with_2"></a>
* [STSW-STM8069](https://www.st.com/en/embedded-software/stsw-stm8069.html) - STM8S/A Standard peripheral library <a name="build_with_3"></a>


### Authors: <a name="authors"></a>
* <img src="https://avatars2.githubusercontent.com/u/61043468?s=400&v=4" width="24" height="24"/> [__Max [MB6718] Bee__](https://github.com/MB6718)


### License: <a name="license"></a>
The product is distributed under the GNU GPL v3.0 license. See [LICENSE](https://github.com/MB6718/USB-WatchDog-firmware/blob/main/LICENSE) file for more complete details.


&copy; 2020 Designed by MB6718
