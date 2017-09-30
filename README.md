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
