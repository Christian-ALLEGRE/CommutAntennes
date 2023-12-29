# CommutAntennes
Antennas switcher using RPIPico and TFT 2"8 TouchScreen

V1.2 : 
  - The TFT 2"8 TouchScreen uses parallel 8 bits communication with RpiPico
  - PCB was manufactured by JLCPcb using Kicad/Ampli144MHz.zip (2023/09/14)
  - RpiPico Program V1.2 UI was design to manage :
    - The antenna switcher : 2 coax relay (HF and VHF)
    - A futur 144MHz 500W Power amplifier (not yet build at this time)
  
Updated on 29/12/2023 : 
  - The source code is now compatible with PlatformIO
    - CommutAntenneRpiPico-V1.2a.ino becomes src/main.cpp
    - The requested libraries were put in lib directory
    - a few lines where commented in GFX_Library_for_Arduino and TouchScreenOnRpPico
      to allow those libraries to compile (search for// F4LAA comment to find them)

