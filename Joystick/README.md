# Joystick Unit

- [Joystick Unit](#joystick-unit)
  - [Major changes I made to the upstream model:](#major-changes-i-made-to-the-upstream-model)
    - [Different \& multiple MCUs](#different--multiple-mcus)
    - [No custom PCBs](#no-custom-pcbs)
    - [No Z-Axis](#no-z-axis)
    - [No LEDs at the top](#no-leds-at-the-top)
  - [Tolerances and connecting parts together](#tolerances-and-connecting-parts-together)
  - [3D Models](#3d-models)
      - [Z-Axis Connector Screw](#z-axis-connector-screw)
      - [Gimbal Mainframe \& Case](#gimbal-mainframe--case)


## Major changes I made to the upstream model:

### Different & multiple MCUs
I'm using multiple and different microcontrollers for this project. I kinda disliked the use of a VGA 9 pin connector between the stick and the base, and would have preferred to go with something smaller, like GX connectors.
I also was unaware that 9+ pin GX connectors exist, so instead I opted to use two microcontrollers for this project, instead of just one:

- an Arduino Nano inside the stick itself, reading all the inputs from the stick (buttons, 5way switches, joystick)
- an STM32F401CC Black Pill board inside the base, reading the hall sensors and displaying a Joystick HID device via USB.

I opted for an STM32F401CC inside the base since it has a USB stack that is already (kinda) supported by some Arduino libraries, and I had one on hand.
The two board communicate over Serial with a custom protocol, utilizing one of the many Serial interfaces of the STM board, in addition to the one published over USB.

### No custom PCBs
The original model utilizes many custom made PCBs, which can probably be cheaply ordered, but I didn't want to spend the money nor wait for them to arrive.
Also, if I had to use the original PCBs, I'd need to make sure to get parts exactly as in the original project, to make sure the footprints align, and custom modifications would be harder.
So instead, I opted to handwire everything.
It's certainly possible, but a bit nervewracking, since you dont have a ton of space inside the joystick, especially the head, where the majority of the parts are.
Since I want to be able to somewhat take the device apart again, to allow for easier repairs or future mods, I also split the wiring into many parts.
To connect the individual parts, small connectors are certainly a must, my first idea of using Dupont connectors proved far too big (and annoying), so I opted for JST connectors instead.

### No Z-Axis
In my head, joysticks should not rotate in my hand while I'm using them, therefore I removed the Z-Axis functionality from the stick.
It saves a bit of space which is needed for e.g. the Arduino inside the stick, and makes the construction a bit easier.
Originally I also didn't see the need for it, but while building I realized there would have been some applications for the Z-Axis in some games.
For now, I kept it out, but this is the good part about DIYing this: I can always add the Z-axis back in, if I want :D

### No LEDs at the top
As I am not [Banjer, the original author](https://github.com/Banjer21/DIY-HOTAS), I removed the LEDs at the top of the stick, along with the "B"-shaped hole in the top.
This is also very much needed to make space inside the joystick head for a lot of wires.

## Tolerances and connecting parts together

The original model does feature some tight tolerances.
Even if your printer is great and you use the exact same parts, you will likely have to post process the printed parts a bit here and there.
I can only recommend using some files and possibly a power drill to some parts.
There were no instructions on what screws etc. to use, I went with mostly M2 screws, and some M3 screws for big parts.
Some of the screw holes are a bit too tight for M2 screws, which may be intentional, so you can screw the M2 screws directly into the printed plastic.
I did not like that, so I wen't down the path of inserting about 60+ M2 heat inserts into the prints, so I have things to screw into everywhere.
It's really not needed, and I'm not sure if I would recommend it, but it does work, even if it takes a long really time (and a lot of effort).
It may make frequent disassemblies / reassemblies a bit easier, with no plastic screwholes to wear out.
If you use inserts, I recommend drilling the holes a bit bigger (or just making them bigger before printing), so your inserts don't get gummed up with molten plastic.
(Happened to me several times.)

## 3D Models

I printed most of the 3D models from the `Righthanded Updated` folder of the original repo once, with some custom made models or replacements for some parts thrown in.
When I have the time, I'll reorganize the stl-files a bit, and add my own here as well.
Below are some notes on individual parts.

#### Z-Axis Connector Screw

For some reason when printing this at original size, it wouldn't fit over the screw for me, so I printed this at 105% scale on the X and Y axis. That fit the screws nicely.

#### Gimbal Mainframe & Case

The entire project is designed so well by the original author, but here they definitely screwed up.
If you print the original Case and Gimbal Mainframe, while putting them together, you will figure out that the Mainframe intersects the Case in two corners near the Y-Axis.
I had to file off quite a bit of both of these edges (you can see the infill in those corners now), to make it fit.
Do yourself a favor, and before printing these, go into the 3D model and make sure those parts fit together without intersecting, otherwise you'll have to file quite a lot. :D