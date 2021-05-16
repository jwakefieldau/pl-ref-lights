# pl-ref-lights
Powerlifting referee lights, light automatically when decision complete, Arduino-based
## Rationale
Gyms running powerlifting comps need light systems, and I wanted an electronics project that was challenging but achievable given my limited experience with electronics.  Many gyms use a simple circuit, master switch-based system with domestic electrical light wiring parts, which does not strictly satisfy the requirement of illuminating lights all together when a complete decision is entered.

I had previously built a software-based system, using Bluetooth controllers, but the code was quite large and complex, and Bluetooth proved an unwieldy nightmware to work with.  This is a much more stable, simple, reliable system.
## System overview
- The heart of the system is an Arduino Uno, which implements all of the logic and signalling.
- The referees' hand controls consist of 4-leg tactile momentary switches mounted on plastic "jiffy" boxes, connected to cat5e jacks.
- The hand controls connect to the base station via cat5/6 cables.
- The lights are 6 automobile side marker LED lights (3 white, 3 red), capable of running with 10-30VDC, powered by 12VDC.
- The lights are switched by an 8-channel relay module, controlled by the Arduino.
## System description (this got a bit detailed, move this out to other sections)
- The heart of the system is an Arduino Uno, which implements all of the logic and signalling.
- The referees submit decisions via 4-leg tactile momentary switches mounted on plastic "jiffy" boxes, connected to cat5e jacks.
    - Using cat5e jacks allows cat5-6 cables of the required length to be used to connect the referees' hand controls to the base box.
    - The specific cat5e jacks used accommodate the 20 AWG wire used in Arduino jumper cables, which are used to connect the cat5e jacks to the momentary switches and the Arduino. 
    - 4-leg switches, in which the opposable legs of each pair are permanently connected, and the pairs are connected by the switching action, allow a common ground to be daisy-chained between each switch on each hand control because two legs can be used for ground, with one switch wired to the appropriate pin on the cat5e jack.
    - One of the legs on the other pair of each switch is the positive/signal and is wired to the cat5e jack.
    - Each hand control has a corresponding cat5e jack in the base box, with the appropriate signal pins connected to the Arduino digital I/O pins.
    - The ground pins in the cat5e jacks are daisy-chained via "double-termination", with one jack connected to the Arduino I/O ground pin.
- The lights are 6 automobile side marker LED lights (3 white, 3 red), capable of running with 10-30VDC, powered by 12VDC.
    - Each light has a relay wired in between it and the load, signalled from the Arduino's digital I/O pins.
    - The normally-open and common screw terminals of each relay are used, so that the light will be energised when signal voltage is supplied to actuate the relay.
    - The relays live in an 8-channel relay module, which itself is powered by 12VDC, 2 relays are unused/spare.
    - The relay board has additional electronics such as flyback diodes, which prevent inductive load due to "back EMF" from relay actuation from damaging the Arduino.
- Power is supplied by a **regulated** 12VDC 2.5A "plug pack".
    - a 1-female to 4-male splitter, two of which are connected to 2 further splitters, is used to distribute this power within the base box.
    - The 8 male plugs in use - 1 Arduino, 1 relay board, 6 lights (via relays) are plugged into female-to-screw-terminal plugs, allowing separate routing of positive and ground wires.
    - The Arduino is connected via its on-board barrel jack, and runs at 5V via its on-board voltage regulator.
    - The relay board is connected via Arduino jumper wires, one for positive, one for ground.
    - Ground for the lights connect directly to a 12 way screw terminal join block.
    - Positive for the lights connect to the normally-open screw terminal of the relay, with the relay's common screw terminal connecting to aforementioned terminal block.
- The lights sit on the outside of the base box, and their provided power cables run into the box and terminate on the screw terminal block.
## Parts list
For all large-quantity items that are cheap per-unit, I recommend buying plenty of spares.

- 12VDC 2.5A "plug pack" power supply, centre positive
    - note, all DC plugs and sockets should be/accommodate 2.1mm 
- 3x female to 4 way male DC splitters
- 8x female to screw terminal DC adapters, centre positive
- Arduino Uno
- USB A to B cable (for programming Arduino)
- 6x Cat5e 110-block keystone jacks
    - you could use Krone jacks if you have a Krone punchdown tool and not a 110-block tool   
- 7x 4-leg tactile momentary switches 
- 8-channel Duinotech relay board
- 1 reel 28 AWG hookup wire in each colour:
    - yellow
    - black
- >= 20 male to male Arduino jumper wires, with square-shaped plug housing
    - I recommend getting loads of these so that you have the lengths and colours you need/want
    - in theory, if you didn't want to test your switches and hand controller jacks on a breadboard, you could just use any hookup wire thin enough to fit the keystone jacks for those; the bare minimum is 8 wires terminated male on one end, to plug into the Arduino.  
    - rounded plug housing is ok for breadboard testing, but they cannot fit in large numbers next to one another on the Arduino.
- >= 8 male to female Arduino jumper wires, with square-shaped plug housing
    - Again, get loads of these, for the same reasons as above.
- 12 way screw terminal joining block
- 3x red and 3x white automotive side marker 12VDC LED globes

**TODO**
- bootlace ferrules
- standoffs
- boxes
- screws
- solder

## Tools list
- Wire cutters (I used wire cutters on my pliers, and many other wire tools have cutters)
- Wire strippers
- 110-block punchdown tool
- Insulated terminal termination tool ("bootlace crimp tool")
- Screwdriver set
- Soldering station
    - Soldering iron
    - Silicon soldering mat
    - Soldering iron stand
    - Third hand
- Breadboard (for testing switches and hand control jacks before soldering)
    - needs to be at least large enough to accommodate 7 momentary switches sitting astride a valley with room forward and behind for wiring  
## Assembly details
## Arduino code
