# Joystick Unit

- [Joystick Unit](#joystick-unit)
  - [Major changes I made to the upstream model:](#major-changes-i-made-to-the-upstream-model)
    - [Different \& multiple MCUs](#different--multiple-mcus)
    - [No custom PCBs](#no-custom-pcbs)
    - [No Z-Axis](#no-z-axis)
    - [No LEDs at the top](#no-leds-at-the-top)
  - [Tolerances and connecting parts together](#tolerances-and-connecting-parts-together)
  - [Primary trigger spring](#primary-trigger-spring)
  - [Different base](#different-base)
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
Now that I am almost done with the stick, I have to reiterate: There is not a lot of space inside the stick.
It might seem like much when you start, but throw in 4 five-way switches, a joystick and 9 additional buttons you have to wire together to a matrix, and it gets crammed very fast.
It is a wise idea to plan your wiring first, I had to rewire the joystick head once, because all the wires and connectors wouldn't fit inside.

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

## Primary trigger spring

The primary trigger is double-action (very cool), but requires some fine spring adjustments to get it to work, as (from my understanding), a spring is supposed to push the first pushbutton while compressing, and the peg presses the second stage pushbutton once fully pushed.
I couldn't find a fitting spring that would push the button when slightly compressed, so I went with a limitswitch (without the little metal bar) for the first stage, as that has a much lower required activation force.
A spring pushes the limit switch when slightly compressed, and the peg presses a push button when fully pressed, encompasing the two stages of the primary trigger.

## Different base

After much of trial and error, due to the usage of cheap-ish bearings and different springs, I could not get the original gimbal to auto-center with my stick.
I attempted to create my own cams and tried two different springs, but neither would work properly.
In the end, I went ahead and built a [different base by Miramaksimus](https://github.com/Miramaksimus/HOTAS-Joystick-Soleil) which worked way better for me.
I adapted some of the parts to work with the bearings, magnets and parts I had on hand.
Since the licenses are compatible, I added my modified STLs to this repo as well.

## 3D Models

I've merged the original `Righthanded Updated` and `Righthanded joystick` STL folders, and added my own additions in there as well.
See the README in that folder for more details, in addition to the notes below.

#### Z-Axis Connector Screw

For some reason when printing this at original size, it wouldn't fit over the screw for me, so I printed this at 105% scale on the X and Y axis. That fit the screws nicely.

#### Gimbal Mainframe & Case

I have to amend my previous statement here, I think the updated files introduced this error, as there is more volume in the "updated" one-part base compared to the original four-part base.
If you print the updated Case and Gimbal Mainframe, while putting them together, you will figure out that the Mainframe intersects the Case in two corners near the Y-Axis.
I had to file off quite a bit of both of these edges (you can see the infill in those corners now), to make it fit.
Do yourself a favor, and before printing these, go into the 3D model and make sure those parts fit together without intersecting, otherwise you'll have to file quite a lot. :D