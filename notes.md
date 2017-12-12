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


# Stepper Motors

IGNORE THE DATASHEET!

Red-Black and Green-Blue are the coil pairs.

# Running Demo

Example:
python svgParser.py method-draw-image.svg 0 0 100 100

call python on our script with the arguments:
* image to draw
* minimum x value
* minimum y value
* maximum x value
* maximum y value

This will change the cpp main file to have the path that will draw what you want. Theoreticall, flashing that code will draw what you want where you want.


## To get your SVG ready for plotting:
    Load your SVG in Method Draw http://editor.method.ac (File > Open Image)
    Ungroup your elements (Object > Ungroup elements) you might have to do this more than once.
    Select your path
    Reorient the path (Object > Reorient Paths).
    Save your image (File > Save Image) If it appears in a new window you can right click and "Save Image as..."

