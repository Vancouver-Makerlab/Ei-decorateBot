This archive contains all necessary files to run the EI Plotter A fork of the Eggplotter which a clone of the Eggbot 

:) yay opensource!


Getting started


Inkscape related files

Install them in your Inkscape directory and keep the same directory structure, e.g. in C:\Program Files\Inkscape\

There is a template for creating your own designs, and there are two example files. One is a flower design and the other one is a test pattern to adjust backlash compensation. Finally, there is the modified EggBot extension.


Arduino related files

Of course, the Arduino sketch can be stored wherever suits you best. Note that the external libraries AccelStepper and Bounce2 are not part of this package and can be downloaded separately from:

AccelStepper - http://www.airspayce.com/mikem/arduino/AccelStepper/
Bounce2 - https://github.com/thomasfredericks/Bounce2






Artchive of blog explaining the concept: (will be converted to mark down soon)


Features
Plots on spheres and eggs, similar to the EggBot
Uses low-cost components (total cost around 25 €)
Software is written as an Arduino sketch
But: NOT as accurate as the original EggBot


Introduction

I have admired the idea of the EggBot for quite some time. The original device is an art project of Bruce Shapiro, apparently conceived in 1990. Read about the history of it here.

You can get a kit to build your own EggBot from Evil Mad Scientist. But unfortunately, it doesn't come cheap. The basic version is about 200 $ in the US and about 210 € in Europe. For me, that is too much money for not enough fun.

But eventually I decided to look into the details of the EggBot. It consists of some structural parts (mainly plywood), two stepper motors, one hobby servo and the "EiBotBoard" control electronics. That's not all too complex, right? I was sure that I could re-create my on version of this at a much lower price. So I started this project.

Prior Art
Since I didn't want to re-invent the wheel, I trawled the internet for other projects that had attempted the same. Although I didn't find quite the project that I was planning, a few projects were inspiring.

My goals were:
Use the Inkscape software with EggBot extension to plot on eggs
Use an Arduino instead of the EiBotBoard
Use 28BYJ-48 stepper motors with ULN2003 driver boards
Use mechanical components from my local DIY shop
The projects and pages that I found useful were:
The SphereBot project - great, but uses normal steppers, 3D printed parts and the firmware speaks GCode only
The Spherebot Russian Roulette page -  with some tips on the SphereBot plus a list of firmwares for the Arduino
The EggDuino (firmware / project page) project -  this was very close to what I wanted to do, but uses standard steppers, not the type that I wanted to use. Analysing the firmware helped a lot, though - great project!
The CNC-Eggbot project - I found great tips for the mechanical side of things, especially the drill stops are genius! (But my advice would be not to use the name "Eggbot" for this project, since it has no connection to the original project and MadEvilScientists have made it clear that you should use alternative names for clones)
Of course the EBB command reference was extremely useful, too.
The control board
The original project uses the EiBotBoard to control the plotter. This board does all the hardware related jobs: control of steppers and pen servo, plus the sensing of the PAUSE button. It is connected to a PC via USB cable. All this can just as well be done by an Arduino, apart from actually driving the steppers.

I decided to use an Arduino Nano clone for this. I needed 2 x 4 pins for my steppers, one pin for the servo and one input pin for the button. An Arduino Nano clone with ATMega328P and CH340 serial interface can be ordered from China for under 2 €. But don't mix it up with the Arduino Micro, which has no USB interface on board.

The Firmware
I do like the EggDuino firmware, but I needed to modify it for my type of steppers. And I always have difficulties to read someone else's code. So I wrote my own. I did make use of some very useful libraries, specifically:
The AccelStepper library, which is great as a universal stepper control library. It does support my low-cost steppers
The Bounce2 library to debounce the PAUSE button
Install both libraries to your Arduino IDE in order to successfully compile the firmware.

Limitations:
Please note that my firmware does not implement all the commands of the EiBotBoard. While analysing the Inkscape extension I realised, that it only uses a very limited subset of the commands that are available. I found a debug switch in the Python code ("bDryRun"), that dumps the complete sequence of commands to a text file instead of sending it out to the EggBot. So I was able to see which commands are used and which aren't. This meant that I got away with less work. For example, none of the node counting commands are used. So I didn't implement them.

Also the EM command, which switches the steppers on and off makes no sense with geared motors. You can't rotate them manually anyway. In addition, my pen servo always moves at full speed. The servo rate settings are ignored.

I have got a feeling that over time, the EiBotBoard grew into something more than just the control electronics for the EggBot. So it has a lot of new features that are not used by the Inkscape EggBot extension.

The funny thing was, that the further my firmware advanced, the more it resembled the EggDuino firmware. And whenever I ran into problems, I peeked at it and I gradually understood it better. In the end, I probably would have been able to modify it for my purposes...

Actuators
I used two geared stepper motors 28BYJ-48 with driver boards based on the ULN2003 IC. These motors have only 64 steps (half stepping), not 200, like the original ones. But due to the integrated gears, you get about 4076 steps per one full turn. The original motors can be controlled with microstepping and you therefore get 3200 steps per one full revolution.

I was not aware about any disadvantages of the integrated gears. But in fact, they introduce significant backlash, which means that you get inaccuracies due to the fact that when you change direction on the motor, the output shaft of the gearbox doesn't move for a few degrees. And this translates to a few millimetres on the surface of the egg - not ideal. 

To raise and lower the pen, I used a standard 9g servo, probably the same as the EggBot. Nothing special there.

Wiring it all up
Since the motors don't draw much current in this application, it is possible to power them via the PC's USB port. So no extra power supply is required. 

The Arduino Nano connects to the PC via USB cable, no surprise there. The two ULN2003 driver boards for the steppers are connected to digital Arduino pins like this:

Pen motor: IN1 - IN4 to D2 - D5
Egg motor: IN1 - IN4 to D6 - D9

Each driver board also needs 5V and GND.

The servo has three pins: Black / Red / White (sometimes Brown / Red / Yellow). Black / Brown goes to GND, Red goes to 5V and Yellow / White goes to D13.

The button goes to GND with one end and to D12 with the other end.

Because the Arduino has no power rail that all the actuators can be connected to, I made a small piggyback PCB for this from perfboard. The PAUSE button is also on this perfboard. The board also doubles as adapter for the servo connector and to carry the button.

Since I only needed two IO pins from the Arduino (button and servo), I decided to use the ISP header for this. Amongst its 6 pins are 5V, GND, D12 and D13. See Ardino Nano schematic (PDF warning) for the pin-out (Pin1 = D12, Pin2 = D13, Pin2 = 5V and Pin6 = GND). 

Mechanics
This is my weak spot. I am not good with mechanics and I don't own a workshop with 3D printers, routers, lathes and all the rest. I do have a simple drill press, but for this project you could also use a normal electrical drill. A metal saw is also required.

Please read to the end, there are some things that I would do differently, if I made this again. 

The Frame
In my local DIY centre I got plywood in size A5 (20 cm x 14.8 cm), 4 mm thick. I also got 1 metre of threaded rod, M6 with matching nuts and washers (24 each). I cut the rod into 5 pieces, 20 cm long. Out of all that I made the frame.

The panel on the left has a hole 10 mm to fit the motor shaft, the panel on the right has a 8 mm hole for the opposite support. See the drawing below for details.




The panel to hold the pen arm motor is another piece of plywood, 10 cm wide and reinforced by two parts of square timber 20x20 mm. The backplate is slid on the to backside rods after drilling holes in the square pieces of wood.



Both motors are inserted into their respective shaft holes and secured with M4 screws. 

 

The Egg Coupler
So far, things were straight forward, but for the egg coupler initially I had no good idea. But I had bought a set of three drill stops, as advised in the "CNC-eggbot" project. The set I got had three sizes: 10, 8 and 6 mm. I took the 8 mm ring and inserted a piece of aluminium tube in it. That tube also came from the DIY centre. I cut a tube 45 mm long. I also drilled a hole in it so that the set screw would go through. I was then able to clamp the tube to the egg motor shaft. I inserted a wooden dowel (5 mm) in the other side of the tube and attached a bottle cap with a small wood screw. In the bottle cap, there are pieces of foam sealing tape.



On the right side panel I fixed an "adjustable levelling foot M10 with T nuts, that I had in a box with bits and pieces. I am not sure where it came from, possible with an IKEA kitchen cabinet.



The foot rotates freely and with little friction. There is a little indent to centre the egg . Perfect. 

  

The Pen Arm 
This is probably the most complex part. I made it from scraps of 6 mm plywood. First I cut a strip 20 mm wide, 140 mm long. Then I divided it in sections of 90, 20 and 30 mm. I connected the 90 and 20 mm parts through a 90° dovetail joint with wood glue and added two holes for screws M4 (see drawing).

The pen holder is connected to the arm by a hinge made of a thin plastic strip from some packaging (grey rectangle in the drawing). It is stabilised by adding small pieces of breadboard on top. Without the perfboard, the joint was far to wobbly.

The 30 mm part also has a 10 mm hole for the pen. The 10 mm drill stop is glued there. The 6 mm drill stop is super-glued to the back of the arm as shown in the drawing. I probably should have positioned it a little lower, because the pen arm sometimes collides with the egg coupler. Use your discretion.

The final work on the pen arm was the attachment of the pen lifting servo. This is done much like on the original EggBot, the servo is fixed with one of the screws that come with it. I filed a small indentation into the arm to give it some support (see photos).





Pen arm - top view


Pen arm - left; notice the indentation for the servo



Pen arm -right view

Once all components were built, all could be installed and finally tested!!!


Inkscape and the EggBot extension
Unfortunately, there are a few problems with the EggBot extension. I used version 2.5.0A as was recommended. I had to make some modifications to the Python code.

As was hinted in the EggDuino project, the code has a bug in Windows, with the effect that it crashes if no real EiBotBoard is installed in the system. A few lines of code have to be commented out.

Then, for some reason, Python sets the DTR serial interface line when it starts talking to a serial device. Even if it is not asked to do so. On the Arduino Nano, this signal is used to reset the device, This isn't too much of a problem, but sometimes the servo behaves a bit unexpected, because after reset it goes to its default position.

I had to modify the extension a little in order to allow for longer time-out, because the Arduino takes about 2 seconds before it comes to life.

And finally I had to change the dimensions of the drawing space so that wrap-around events would be handled correctly.

The modified version of the EggBot extension is packaged with my Arduino sketch. First, install the full "official" extension 2.5.0A. Then just copy the modified eggbot.py file to the correct Inkscape directory (e.g. C:\Program Files\Inkscape\share\extensions in Windows).     


Due to the different dimensions of my EggPlotter drawing space (4076 x 900 pixels) original EggBot designs that you might have downloaded don't fit around an egg. You have to stretch them a little and I included a template for EggPlotter files.

Other than that, the EggPlotter works just like the original. Check out all the relevant documentation.

Initial Results and Experience
After I had ironed out some minor bugs in my code, the plotter worked. But I had to accept that the accuracy was not expected. I had underestimated the backlash problem that you get by using geared motors. So I added a backlash compensation feature. Whenever a motor changes direction, I add an offset to its next motion. This improves things significantly. But the EggPlotter's accuracy is not as good as the EggBots. Here is a picture to demonstrate the effect of the backlash compensation:



On the left there is a table tennis ball that has no backlash compensation for the egg motor (up/down), whereas left/right (the pen arm) works reasonably well. On the right, the effect of the compensation is clearly visible. In fact, there is a slight over-compensation.

Unfortunately, backlash doesn't seem to be constant. I suspect it depends on size and surface of the ball/egg, probably due to specific force needed to move the egg. For reasons I don't understand, backlash seems to be less of a problem on the pen arm motor.

There are two lines in the Arduino code:
#define compensatePen 10
#define compensateEgg 30

Experiment with the backlash test pattern to get best results.

Considering the low cost of my solution, I am satisfied with the result.

  





Room for improvement 
Stepper Motors
The backlash of the geared motor is probably the main reason for all the inaccuracies of my EggPlotter. Should I ever build a second version, it will use"normal" stepper motors. This is the most important point.

RESET Issue
The Arduino resets when serial communication is first started. Sometimes this makes the pen servo to lower the pen and I get unwanted dots. Not too much of a problem, but this could be eliminated by interrupting the DTR signal on the arduino. Then again, I believe I red that later versions of the Python serial library don't have that problem.

Pen Arm
The pen holder should be aligned better. I made is a little too short. With larger eggs, there is a danger of the pen arm crashing into the egg coupler. Also, the holder doesn't meet the egg right on the top of the egg. This means that lines aren't quite straight, but bend a little.

The Tilting Pen Raising Mechanism
This is  a problem that the original EggBot must also suffer from. It is always a little tricky to insert the pen correctly. If it isn't inserted deep enough, the pen can lift occasionally and it would then leave white spots. If however you insert it to deep, the pen "slides" off the egg. It isn't at the top of the egg anymore, but moves down and so creates these curved lines.

A solution would be to somehow build a mechanism that would lift the pen absolutely straight. Then, there wouldn't be any chance for the pen to move away from the egg's centre. 

Firmware
Right now the firmware isn't implementing the full function set of the EiBotBoard. This is no problem now, but could mean that later versions of the Inkscape extension could be incompatible.
 

A Spring Loaded Egg Coupler
My Egg Coupler is a little simplistic. I works, but it might be more convenient to have a spring loaded mechanism like the original. It would simplify usage a little.

Final Words 
And that's it.The project was fun to make and is fun to use. As mentioned, it isn't quite as accurate as it should be. But it was easy to build and really cheap. It might make a nice project to build with students and it certainly helps you to gain experience in several fields, like mechanics, stepper motors, use of a vector based drawing program, Arduino programming and woodworking, to mention the most important ones. All at very little cost if you source electronics parts in China.

Find all the source code, drawings, etc. on my download page