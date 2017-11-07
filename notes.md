# Coordinate Systems
**width** = int mm

**string lengths:** int stepper_motors_steps

**mm_to_steps:** float [magic number from calibration]

### artboard:
**x,y:** floats 0-100

**top left** - [0,0]

**top right** - [100, 100]


# Installing platformIO:
```
sudo pip install -U platformio
wget https://raw.githubusercontent.com/platformio/platformio-core/develop/scripts/99-platformio-udev.rules

sudo cp 99-platformio-udev.rules /etc/udev/rules.d/99-platformio-udev.rules
sudo service udev restart
```

# Find boards:
```
platformio boards
```
Arduino uno is ```uno```

## Init a project in a directory:
```
cd /path/to/project
platformio init --board uno
```

## Write code:
Write code in /src/main.cpp

## Upload:
```
platformio run --target upload
```

## Monitor Serial:
```
platformio device monitor
```

## Upload and monitor:
```
platformio run --target upload ;and platformio device monitor
```


# Linux Laser Cutting Toolchain

Visicut + inkscape for lasercutting

# Linux 3D print slicer
<jared fill this in>
