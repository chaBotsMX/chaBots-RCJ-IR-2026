# chaBotsMX — RoboCup Junior Soccer Infrared 2026

**Contact:** contacto@chabots.mx · [chabots.mx](https://chabots.mx)
 
---
 
## About
 
This repository contains all open-source hardware and software files for our 2026 competition robots. Everything here is designed with accessibility in mind — locally sourceable materials, additive manufacturing, and no specialized equipment required.
 
For a full technical breakdown, see our [Team Description Paper](https://github.com/user-attachments/files/27860903/chaBotsMX_TDP.2.pdf).

 
---
 
## Repository Structure
 
```
├── Mechanical/         # 3D-printable parts (PET-CF17 / Polycarbonate)
│   ├── Dribbler/       # Dribbler assembly for 42mm IR ball
│   ├── Media/          # Images and PDFs of the resulting models
│   ├── V1/             # Models & assembly of our v1 robot
│   ├── V2/             # Models & assembly of our v2 robot
│   └── Wheel/          # Custom omnidirectional wheel system
│
├── Electrical/         # Schematics and PCB layouts
│   ├── BallSensor16/   # BPV10NF photodiode + TSSP58038 IR receiver
│   ├── LightArray16/   
│   ├── LightBarrier/   
│   ├── LineConnector/  
│   ├── LineSensor32/   # ALS-PT19 + LMV331 comparator circuit
│   ├── Main-2026/      # Main PCB
│   ├── Media/          # Images and PDFs of the PCB layouts
│   ├── OpenMVMount/    
│   └── SideLine/       
│
└── src/
    ├── code/           # PlatformIO Teensy Firmware
    └── vision/         # OpenMV scripts
```
 
---
 
## License
 
Hardware and software are released under MIT License](LICENSE) in the spirit of open knowledge sharing within the RoboCup community.
 
---
 
*Competing since 2015 -[:]*
