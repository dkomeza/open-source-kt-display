[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

# Open-Source KT Display
Open source display firmware for E-Bike KT controllers. Made for Arduino with compatible SPI Display. 

<br>

## Table of contents

- [Open-Source KT Display](#open-source-kt-display)
  - [Table of contents](#table-of-contents)
  - [KT Controller communication protocol](#kt-controller-communication-protocol)
    - [Controller to Display](#controller-to-display)
    - [Display to Controller](#display-to-controller)
      - [Wheel size table](#wheel-size-table)

<br><br>

## KT Controller communication protocol

Special thank you to user *@obelix662000* at endless-sphere.com for sharing the docs in [This post](https://endless-sphere.com/forums/viewtopic.php?f=2&t=73471). However there seem to be some problems regarding the B6 Checksum calculation (not sure yet). Apparently the formula is not always correct and varies from display to display. I will have to test this myself.
<br><br>

### Controller to Display
<br>

```
Packet consist of 12 bytes. 9600 baud, 8-n-1, byte-by-byte, no separators 

B0	B1	B2	B3	B4	B5	B6	B7	B8	B9	B10	B11

(e.g: 65 16 0 0 139 0 164 2 13 0 0 0)

B0: 65 dec (0x41)
B1: battery level: 0: empty box, 1: border flashing, 2: animated charging, 3: empty, 4: 1 bar, 8: 2 bars, 16: 4 bars (full),
B2: 0 dec (0x00)
B3,B4: speed, wheel rotation period, ms; period(ms)=B3*256+B4;
B5 error info display: 0x20: "0info", 0x21: "6info", 0x22: "1info", 0x23: "2info", 0x24: "3info", 0x25: "0info", 0x26: "4info", 0x28: "0info"
B6: CRC: xor B0,B1,B2,B3,B4,B5,B7,B8,B9,B10,B11
B7: moving mode indication, bit
b7b6b5b4 b3b2b1b0
. . . .  . . . m0      if set animated circle "throttle"
. . . .  m0 . . .      if set "C" (cruise) is shown
. . . m0  . . . .      if set "assist" shown
. . m0 .  . . . .      if brake icon shown
B8: power in 13 wt increments (48V version of the controller)
B9: motor temperature, can be negative or positive,T(C)=(int8)B8+15,
    if temperature > 120C LCD screen is flashing.
    e.g 0xDA T=-23C, 0x34 T=67C
B10: 0
B11: 0
```
<br><br>

### Display to Controller
<br>

```
Packet consist of 13 bytes. 9600 baud, 8-n-1, byte-by-byte 

B0	B1	B2	B3	B4	B5	B6	B7	B8	B9	B10	B11	B12

(e.g: 12 0 149 160 41 102 18 74 4 20 0 50 14)

for the P and C parameters description please see S-LCD3 user manual available at the bmsbattery.com

B0: parameter P5.
B1: assist level, front light. 
b7b6b5b4 b3b2b1b0
. . . .  . l2l1l0     assist level 0-5, 6-walk (long push down arrow)
f0. . .  . . . .      bit (mask 0x80) front light, display backlight
B3: parameter P1.
B2 and B4 max speed, wheel size, P2,P3,P4
B2: b7b6b5b4 b3b2b1b0 and B4:b7b6b5b4 b3b2b1b0  
    s4s3s2s1 s0. . .      . . s5.  . . . .   max speed minus 10 km/h;   6bit
    . . . .  . w4w3w2     w1w0. .  . . . .  wheel size:0x0e-10",
    . . . .  . . . .      . . . .  . . . .  0x02-12", 0x06-14",
    . . . .  . . . .      . . . .  . . . .  0x00-16",0x04-18",
    . . . .  . . . .      . . . .  . . . .  0x08-20", 0x0c-22",
    . . . .  . . . .      . . . .  . . . .  0x10-24", 0x14"-26",
    . . . .  . . . .      . . . .  . . . .  0x18-700c 
    . . . .  . . . .      . . . .  . p2p1p0  par. P2 (B4&&0x07)
    . . . .  . . . .      . . . .  p0. . .   par. P3 (B4&&0x08)
    . . . .  . . . .      . . . p0 . . . .   par. P4 (B4&&0x10)
Example:
    0 1 1 1  1 . . .      . . 0.   . . . .  25km/h (15+10)
    1 1 1 1  0 . . .      . . 0.   . . . .  40km/h (30+10)
    1 0 0 1  0 . . .      . . 1.   . . . .  60km/h (50+10)
B5: CRC = (xor B0,B1,B2,B3,B4,B6,B7,B8,B9,B10,B11,B12) xor 2.
B6: parameters C1 and C2
b7b6b5b4 b3b2b1b0
. . c2c1 c0. . .       param C1 (mask 0x38)
. . . .  . c2c1c0      param C2 (mask 0x07) 
B7: parameter C5 and C14
b7b6b5b4 b3b2b1b0
. . . .  c3c2c1c0      param C5 (mask 0x0F)
1 c1c0.  . . . .       param C14 (mask 0x60)
B8: parameter C4 and C12
b7b6b5b4 b3b2b1b0
c2c1c0.  . . . .       param C4  (mask 0xE0)
. . . .  . c2c1c0       param C12  (mask 0x07)
B9: 20
b7b6b5b4 b3b2b1b0
0 0 0 1  0 1 0 0
B10: parameter C13 + 1
b7b6b5b4 b3b2b1b0
. . . c2 c1c0 . 1       param C13  (mask 0x1C)
B11: 50 dec (0x32)
B12: 14 dec (0x0E)
parameters C3, C6, C7, C8, C9, C10 not sent to MCU

```

#### Wheel size table
```
29 - 30
28 - 28
700C - 24
26 - 20
24 - 16
23 - 12
20 - 8
18 - 4
16 - 0
14 - 6
12 - 2
10 - 14
08 - 10
06 - 18
05 - 22
```


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/dkomeza/open-source-kt-display.svg?style=for-the-badge
[contributors-url]: https://github.com/dkomeza/open-source-kt-display/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/dkomeza/open-source-kt-display.svg?style=for-the-badge
[forks-url]: https://github.com/dkomeza/open-source-kt-display/network/members
[stars-shield]: https://img.shields.io/github/stars/dkomeza/open-source-kt-display.svg?style=for-the-badge
[stars-url]: https://github.com/dkomeza/open-source-kt-display/stargazers
[issues-shield]: https://img.shields.io/github/issues/dkomeza/open-source-kt-display.svg?style=for-the-badge
[issues-url]: https://github.com/dkomeza/open-source-kt-display/issues
[license-shield]: https://img.shields.io/github/license/dkomeza/open-source-kt-display.svg?style=for-the-badge
[license-url]: https://github.com/dkomeza/open-source-kt-display/blob/master/LICENSE.txt