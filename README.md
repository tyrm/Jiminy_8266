# Jiminy_8226
Multipurpose MQTT LED Controller

## Packet Format
```
<CMND[|OP1[|OPn]]>
```
* **CMND** Command
 * 4 Character Command to run on the Jiminy Controller.
* **OP1**/**OPn** Options
 * Max 3 character parameter for the command.

## Commands
### PING
Send PONG response with MAC
```
PING
```
#### Response:
```
PONG|123456789abc
```

### SETP - Set Pixel
Sets a pixel to a color using an RGBW value. Multiple concurrent pixels can be effected by providing multiple RGBW Values. In RGB devices the white value may or may not be ignored.
```
SETP|i|r0|g0|b0|w0[|rN|gN|bN|wN]

i  - 0-999 - Index
r0 - 0-255 - Red Value of pixel i
g0 - 0-255 - Green Value of pixel i
b0 - 0-255 - Blue Value of pixel i
w0 - 0-255 - White Value of pixel i
rN - 0-255 - Red Value of pixel i + N (optional, repeatable)
gN - 0-255 - Green Value of pixel i + N (optional, repeatable)
bN - 0-255 - Blue Value of pixel i + N (optional, repeatable)
wN - 0-255 - White Value of pixel i + N (optional, repeatable)
```

### SETA - Set All pixels
Sets a pixel to a color using an RGBW value. In RGB devices the white value may or may not be ignored.
```
SETA|r|g|b|w

r - 0-255 - Red Value of all pixels
g - 0-255 - Green Value of all pixels
b - 0-255 - Blue Value of all pixels
w - 0-255 - White Value of all pixels
```

### BRIG - Brightness
Sets the brightness of the device. Values between 0 (off) - 255 (oh god my eyes)
```
BRIG|b

b - 0-255 - Brightness
```

### CLRS - Set Color Scheme
```
CLRS|s|r0|g0|b0|w0[|rN|gN|bN|wN]

s  - 0-9   - Scheme
r0 - 0-255 - Red Value of value 0
g0 - 0-255 - Green Value of value 0
b0 - 0-255 - Blue Value of value 0
w0 - 0-255 - White Value of value 0
rN - 0-255 - Red Value of pixel i + N (optional, repeatable)
gN - 0-255 - Green Value of pixel i + N (optional, repeatable)
bN - 0-255 - Blue Value of pixel i + N (optional, repeatable)
wN - 0-255 - White Value of pixel i + N (optional, repeatable)
```
Color schemes are internally stored sets of colors used by programs. There is space for 10 schemes [0-9] which can store up to 10 RGBW values per scheme.

### PATS - Display Pattern
```
PATS|p|s

p - 0-4 - Pattern
s - 0-9 - Scheme
```
Display a pattern based on a color scheme.

Patterns:
* 0 - Alternate colors
* 1 - Gradient
* 2 - Gradient Reversed
* 3 - Segments
* 4 - Segments Reversed
