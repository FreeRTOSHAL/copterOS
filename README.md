Copter OS
======================================

This is a sample Project based on FreeRTOS HAL Contains a platform independent Quatocopter Controller. 
This Repo is tested on Freescale Vybrid VF610. The Board is documented at [http://uniworse.org/vybridmodule/]

FreeRTOS, FreeRTOS.org and the FreeRTOS logo are trademarks of Real Time Engineers Ltd. 

This is a unofficial Repo of FreeRTOS. This part of the FreeRTOS HAL develops at UaS RheinMain. 

Build
-----
Clone this repo with git clone --recursive or init submoudle with 'git submoudle init; git submoudle update;'

Configure Project: 

```sh
make tlm_vf610_defconfig
```

Build Project: 
```sh
make
```
