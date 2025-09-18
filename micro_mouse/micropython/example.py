# File saved as Mazesolver1.py in UKMARS/Python/MakerRP2040 folder
# UKMARSbot running python on maker nano RP2040 board using MicroPython with 4 wall sensors board
# 5V cpu board socket pin removed and 5v and 3.3v lines connected on back of the sensor connector
# R7 10K resistor removed to stop battery voltage feed to CPU
# In Thonny on windows PC  - From Run, Select Interpreter then MicroPython (Raspberry Pi Pico)
# 30:1 gear motors with magnetic encoders from pi hut 18cm = about 4000 pulses on sum of 4 encoders
#
# uses 2 LEDs on sensor board plus the Nano boardpin 18 LED to select programs no 0 to 7
# press PCB switch to advance the program number from 0 through to 7
# - program number is dislayed as 3 bits of binary
# - prog 1 = maze solver, 2 = maze solver with debug on, 3 = phototest,  
# - prog 4 = encodertest, 5 = switchtest, 6 = motortest
# When the PCB tactile button is pressed the switchvalue goes from 0 to 1
#
# To run mazesolver - switch on power, press PCB button once , - or 2 times to put debug on (red LED lit)
# press GP20 button on Nano board to select program being indicated,
# make sure mouse is set in centre of cstart cell press PCB button to store side wall calibration data,
# press GP20 button to start run. Mouse will move back to align with rear wall of start cell, then go ahead.
# Robot pauses for 2 seconds at the end of each move if in debug mode

# to fix next:
#
#  ***********************  ******************************

from machine import Pin, ADC, PWM
import neopixel, time, array
#import time
#import array

# Global constants

WIDTH = 16 # is 16 in full size maze
HEIGHT = 16 # is 16 in full size maze
TABLEWIDTH = 16
TABLEHEIGHT = 16
START = 0 # the strat cell number
MIDDLE = int((TABLEWIDTH * HEIGHT /2) + (WIDTH / 2)) 
NORTH = 1
EAST = 2
SOUTH = 4
WEST = 8
VISITED = 16
EXPLORE = 1
RUNMAZE = 3	
AHEAD = 1
LEFT = 2
RIGHT = 3
ROUND = 4
PRESENT = 1            # item is known to be present
NOTPRESENT = 0         # item is known to be not present
UNKNOWN = 2            # state of item is unknown
OUT = 0                # going OUT from start to centre
BACK = 1               # coming BACK to start from centre
YES = 1
NO = 0
debug = 1
lfrontwallseen = 500    # value to be above for LF if front wall seen from cell boundary
rfrontwallseen = 250    # value to be above for RF if front wall seen from cell boundary
currentcell = 0
speedbias = 1.03

# Global variable definitions
global leftside, leftfront, rightfront, rightside
global l1count, l2count, r1count, r2count, prevleft1, prevleft2, prevright1, prevright2
global bit0, bit1, bit2, bit3
# pin definitions
# Set pins for digital outputs on Maker Pi RP2040
LED_PIN = Pin(8, Pin.OUT) # ext LED on GP18
 
SENSOR1_PIN = Pin(13, Pin.OUT) # 1st Sensor LED on sensor board
SENSOR2_PIN = Pin(12, Pin.OUT) # 2nd Sensor LED on sensor board
SIDETRIGGER_PIN = Pin(16, Pin.OUT) # Trigger for side LEDs on line follow sensor board
FRONTTRIGGER_PIN = Pin(19, Pin.OUT) # Trigger for front LEDs on line follow sensor board
RMOTOR_DIR = Pin(8, Pin.OUT)
LMOTOR_DIR = Pin(7, Pin.OUT) 

PIEZO_PIN = Pin(7, Pin.OUT) # Pin connected to piezo buzzer 

#define PWM motor speed pins
#LMOTOR = machine.Pin(9) # left motor is pin 9
#MOTOR = machine.Pin(17) # right motor is pin 17
LMOTOR_PWM = machine.PWM(machine.Pin(9))
RMOTOR_PWM = machine.PWM(machine.Pin(17))
LMOTOR_PWM.freq(2000)
RMOTOR_PWM.freq(2000)

LMOTOR_PWM.duty_u16(0) # left motor speed - range is 0 to 65535
RMOTOR_PWM.duty_u16(0) # right motor speed - range is 0 to 65535

# Define analogue inputs used on sensor board
Lsidesense = ADC(Pin(29))   # A3
Lfrontsense = ADC(Pin(28))  # A2
Rfrontsense = ADC(Pin(27))  # A1
Rsidesense = ADC(Pin(26))   # A0

# Define RP2040 on-board button with pull up so it goes false when pressed
btn1 = machine.Pin(20, Pin.IN, Pin.PULL_UP)
# define press button switch on UKMARS board next to 4 way switch which should be set to all on
Switch = Pin(14, Pin.IN) # button / switch pin

Leftenc1 = Pin(2, Pin.IN) # encoder pins
Leftenc2 = Pin(4, Pin.IN)
Rightenc1 = Pin(3, Pin.IN)
Rightenc2 = Pin(5, Pin.IN)

def backup():
    RMOTOR_DIR.value(0)  # set the right motor direction to reverse
    LMOTOR_DIR.value(1)  # set the left motor direction to reverse
    LMOTOR_PWM.duty_u16(15000) # left motor speed - range is 0 to 65535
    RMOTOR_PWM.duty_u16(15000) # right motor speed - range is 0 to 65535
    time.sleep (1)
    LMOTOR_PWM.duty_u16(0) # set left motor speed to stopped
    RMOTOR_PWM.duty_u16(0) # set right motor speed to stopped
    RMOTOR_DIR.value(1)  # set the right motor direction back to forward
    LMOTOR_DIR.value(0)  # set the left motor direction back to forward
    return

def beep():   # Play a tone
    PIEZO_PWM = machine.PWM(machine.Pin(22))
    PIEZO_PWM.freq(440)
    PIEZO_PWM.duty_u16(30000) # loudness in range 0 to 65535
    time.sleep(0.5)
    PIEZO_PWM.duty_u16(0)
    
def buttonwait():  # wait for the GP20 button on the nano board to be pressed
    setting = 1
    while (setting == True): # Check button 1 (GP20)
        setting = btn1.value()
    return

def calibrate(): # get mouse aligneed in centre of start cell and save sensor initail values
    global leftside, rightside, leftfront, rightfront, leftinit, rightinit, lfrontinit, rfrontinit
    global ls, lf, rf, rs
    leftinit = rightinit = lfrontinit = rfrontinit = 0
    time.sleep(.1) 
    switchread()
    while(swvalue == 0):  # until rear button is pressed
        photoread()       # read photo sensors
        #print((leftside),(leftfront), (rightfront),(rightside))
        if (ls == rs): # if central put both LEDs on
            SENSOR1_PIN.value(True)
            SENSOR2_PIN.value(True)
        if (ls > rs):  # put LEDs on to indicate if left or right of centre
            SENSOR1_PIN.value(True)
            SENSOR2_PIN.value(False)
        else:
            SENSOR1_PIN.value(False)
            SENSOR2_PIN.value(True)
        switchread()                 # read switch, save values when rear button pressed
    leftinit = ls     # save calibrated values of the 4 sensors
    rightinit = rs
    lfrontinit = lf
    rfrontinit = rf
    if (debug == 1):
        print ("calibrated", ls, lf, rf, rs)

def checkspeed():
    global leftspeed, rightspeed
    if leftspeed > 60000:
        leftspeed = 60000
    if rightspeed > 60000:
        rightspeed = 60000
    if leftspeed < 10:
        leftspeed = 10
    if rightspeed < 10:
        rightspeed = 10
    #if (debug == 1):
        #print ("checkspeed", leftspeed, rightspeed)
    return

def encoderreset():
    global l1count, l2count, r1count, r2count 
    l1count = 0
    l2count = 0
    r1count = 0
    r2count = 0
    
def encodertest():     # 18 cm movement should give just over 200 counts on l1 and r1 and just over 100 on r1 and r2
    global l1count, l2count, r1count, r2count     # making 620 total to move 1 cell forward
    encoderreset()     # set encoder counts back to zero
    while True:
        print((l1count), (l2count), (r1count), (r2count))
        time.sleep(.5)
    return

def flashblue(n):   #
    if (n == 1):
        pixels[1] = (0, 0, 64) # set to blue, 1/4 brightness
        pixels.write()
    if (n == 0):
        pixels[1] = (0, 0, 0)
        pixels.write()

def flashLEDs():   #
    while True:
        SENSOR1_PIN.value(True)
        SENSOR2_PIN.value(True)
        LED_PIN.value(True)
        time.sleep(0.5)
        SENSOR1_PIN.value(False)
        SENSOR2_PIN.value(False)
        LED_PIN.value(False)
        time.sleep(0.5)

def floodclear(): # clear the flood table
    for x in range(256):
        maze[x] = numcells +1

def floodmaze(strt,fin):   # flood the maze from the strt cell to the fin cell
    global maze, walls, floodfail
    floodclear()           # clear the flood table to all 283
    flooded = 0            # set flag to not finished flooding yet
    floodfail = 0          # flag to show if flood failed to complete to end point
    curr = strt            # current cell being processed
    floodval = 0
    maze[strt] = 1         # set start cell flood value to one
    n = 0                  # index for processing list array of cells to say where to add to end of list
    nxt = 0                # pointer to the first unprocessed item on the list
    while (flooded == 0):
        fval = maze[curr]  # get current value of current cell
        if ((walls[curr] & SOUTH) == 0):     # is there a gap to the SOUTH of current cell
            if ((maze[curr - TABLEWIDTH] == 0) | ((fval + 1) < maze[curr - TABLEWIDTH])):
                maze[curr - TABLEWIDTH] = fval + 1    # set flood value in this cell
                proclist[n] = curr-TABLEWIDTH         # save flood cell for future processing
                n = n + 1                        # update processing list number
                if (proclist[n-1] == fin):       # check if finished flooding
                    flooded = 1                  # set flag to stop loop
        if ((walls[curr] & EAST) == 0):      # is there a gap to the EAST of current cell
            if ((maze[curr + 1] == 0) | ((fval + 1) < maze[curr + 1])):
                maze[curr + 1] = fval + 1        # set flood value in this cell
                proclist[n] = curr + 1           # save flood cell for future processing
                n = n + 1                        # update processing list number
                if (proclist[n-1] == fin):           # check if finished flooding
                    flooded = 1                      # set flag to stop loop
        if ((walls[curr] & NORTH) == 0):     # is there a gap to the NORTH of current cell
            if ((maze[curr + TABLEWIDTH] == 0) | ((fval + 1) < maze[curr + TABLEWIDTH])):
                maze[curr + TABLEWIDTH] = fval + 1    # set flood value in this cell
                proclist[n] = curr + TABLEWIDTH       # save flood cell for future processing
                n = n + 1                        # update processing list number
                if (proclist[n-1] == fin):           # check if finished flooding
                       flooded = 1                      # set flag to stop loop
        if ((walls[curr] & WEST) == 0):      # is there a gap to the NORTH of current cell
            if ((maze[curr - 1] == 0) | ((fval + 1) < maze[curr - 1])):
                maze[curr - 1] = fval + 1        # set flood value in this cell
                proclist[n] = curr - 1           # save flood cell for future processing
                n = n + 1                        # update processing list number
                if (proclist[n-1] == fin):       # check if finished flooding
                    flooded = 1                  # set flag to stop loop
        #print (proclist[n-1] , fin)
         # print (strt, fin, nxt, n, proclist)
        
        curr = proclist[nxt]                 # get the location of the next cell to process
        nxt = nxt + 1                        # point to next item to process on the list
        
        if (nxt > n):                        # check if flood unable to continue as no more cells accessible
            floodfail = 1                     # set flood failure status flag
            flooded = 1 # stop  the flooding loop
            if (debug == 1):
                print (strt, fin, nxt, n, proclist)
        #print ("after flood")
        #showflood()
    return                                    # return 
    
        #print (curr, n, nxt, fval, proclist[n-1])
  
    #showflood()
    #showwalls()
    #halt()    

def goahead(dist,endspeed): # dist is in encoder pulses i.e. around 620 total of 4 counts for 180mm
    global l1count, l2count, r1count, r2count, currdist, leftwall, rightwall, frontwall, leftinit, rightinit, ls , rs
    global leftspeed, rightspeed, basespeed, debug, speedbias
    if (debug == 1):
        print ("goahead"," dist",dist, "endspeed",endspeed)
    startdist = currdist # distance from start of cell boundary when called in pulses
    startcount = l1count + l2count + r1count + r2count # vaue of counts when we call this procedure
    gonesum = 0
    pfactor = 2
    nowspeed = basespeed
    while (gonesum < dist):
        #time1 =  time.ticks_us()
        gone = l1count + l2count + r1count + r2count
        gonesum = gone - startcount
        photoread()
        showLEDwalls()       
        #print(dist, gonesum, l1count, l2count, r1count, r2count, startdist, startcount)
        #  we can track walls if present in this cell for the first half of cell only
        #if ((gonesum + currdist) < 1800):
        # check if too close to left wall and if so move away proportionally
        maxdiff = 5000
        if (ls > leftinit ):  # left wall too close
            wallerr = leftinit - ls  # difference from the initial wall reading
            if (wallerr > maxdiff):
                wallerr = maxdiff
            if (wallerr < -maxdiff):
                wallerr = -maxdiff
            rightspeed = nowspeed + int(wallerr * pfactor)
            leftspeed = nowspeed - int(wallerr * pfactor)
            checkspeed()
            LMOTOR_PWM.duty_u16(leftspeed)
            RMOTOR_PWM.duty_u16(rightspeed) 
        # check if too close to right wall and if so move away proportionally
        if (rs > rightinit):   # right wall follow
            wallerr = rightinit - rs  # difference from the initial wall reading
            if (wallerr > maxdiff):
                wallerr = maxdiff
            if (wallerr < -maxdiff):
                wallerr = -maxdiff
            rightspeed = nowspeed - int(wallerr * pfactor)
            leftspeed = nowspeed + int(wallerr * pfactor)
            checkspeed()
            LMOTOR_PWM.duty_u16(leftspeed)
            RMOTOR_PWM.duty_u16(rightspeed)
        pixels[0] = (0, 0, 0) # red pixel off
        pixels.write()   
        
    if (debug == 1):
        print ("goahead done", dist)        
        pause() # stop and wait for 2 seconds then restart at basespeed
    # rest of stub - to be written later to adjust to required end speed 
    return

def goonecellahead(): # go forward one cell
    global heading, currentcell,lf,rf,lfrontwallseen,rfrontwallseen, basespeed, currdist
    if (debug == 1):
        print ("goonecellahead heading", heading, "currentcell", currentcell)
    currdist = 0
    goahead(2150, basespeed) # move 1/2 cell forward
    if ((lf > lfrontwallseen) and (rf > rfrontwallseen)):
            frontwall = PRESENT
            pixels[0] = (0, 128, 0) # set to green, 1/2 brightness
            pixels.write()
            turnaround(0)    # turn around immediately as already in centre of cell
            pixels[0] = (0, 0, 0) # set pixels off
            pixels.write()
    else:
        frontwall = NOTPRESENT
        goahead(2150, basespeed)   #move 1/2 cell forward 
        if (heading == NORTH):
            currentcell = currentcell + TABLEWIDTH # update cell pointer as we move into next cell
        if (heading == SOUTH):
            currentcell = currentcell - TABLEWIDTH # update cell pointer as we move into next cell
        if (heading == EAST):
            currentcell = currentcell + 1 # update cell pointer as we move into next cell
        if (heading == WEST):
            currentcell = currentcell - 1 # update cell pointer as we move into next cell
        # heading stays the same
    return


def halt():      # halt and flash LED
    LMOTOR_PWM.duty_u16(0) # set left motor speed to stopped
    RMOTOR_PWM.duty_u16(0) # set right motor speed to stopped
    print ("halt called")
    flashLEDs()


def LEDtest():
    while True:
        SENSOR1_PIN.value(True)
        SENSOR2_PIN.value(True)
        LED_PIN.value(True)

def leftcount1():
    global l1count
    l1count = l1count+1
    return
def leftcount2():
    global l2count
    l2count = l2count+1
    return

def motortest():      #
    RMOTOR_DIR.value(1)  # set the right motor direction to forward
    LMOTOR_DIR.value(0)  # set the left motor direction to forward
    LMOTOR_PWM.duty_u16(15500) # set left motor speed higher to go straight
    RMOTOR_PWM.duty_u16(15000) # set right motor speed 
    while (True):
        #flashLEDs()
        print((l1count), (l2count), (r1count), (r2count))
        time.sleep(.5)


def nowtime():
    global curr_msec
    curr_msec = round(time.time()*1000)
    
def pause():
    global basespeed
    stop()
    time.sleep(2)
    LMOTOR_PWM.duty_u16(basespeed) # set left motor speed 
    RMOTOR_PWM.duty_u16(basespeed) # set right motor speed 
    
def photoread(): # numbers get higher as wall gets closer
    global leftside, leftfront, rightfront, rightside, ls, lf, rf, rs 
    global leftinit, rightinit, lfrontinit, rfrontinit, lfrontwallseen, rfrontwallseen
    lsideoff = Lsidesense.read_u16() # read side sensor with the LEDs off
    rsideoff = Rsidesense.read_u16() # read side sensor with the LEDs off
    SIDETRIGGER_PIN.value(1)         # switch on side LEDs on sensor board
    time.sleep_us(80)
    leftside = Lsidesense.read_u16()  # read side sensor with the LEDs on
    rightside = Rsidesense.read_u16() # read side sensor with the LEDs on
    SIDETRIGGER_PIN.value(0)
    time.sleep_us(80)
    lfrontoff = Lfrontsense.read_u16() # read front sensors with the LEDs off
    rfrontoff = Rfrontsense.read_u16() # read front sensors with the LEDs off
    FRONTTRIGGER_PIN.value(1)          # switch on front LEDs on sensor board
    time.sleep_us(80)
    leftfront = Lfrontsense.read_u16()  # read front sensors with the LEDs on
    rightfront = Rfrontsense.read_u16() # read front sensors with the LEDs on
    FRONTTRIGGER_PIN.value(0)
    time.sleep_us(80)
    ls = leftside - lsideoff
    rs = rightside - rsideoff
    lf = leftfront - lfrontoff
    rf = rightfront - rfrontoff
    if (ls < 0):
        ls = 0
    if (rs < 0):
        rs = 0    
    if (lf < 0):
        lf = 0
    if (rf < 0):
        rf = 0
    return

def phototest():
    while True:
        global leftside, leftfront, rightfront, rightside, ls, lf, rf, rs, lfrontwallseen, rfrontwallseen
        global leftinit, rightinit, lfrontinit, rfrontinit 
        leftinit = rightinit = lfrontinit = rfrontinit = 0
        photoread()
        if (ls > 1000):
            SENSOR1_PIN.value(True)
        else:
            SENSOR1_PIN.value(False)
        if (rs > 1000):
            SENSOR2_PIN.value(True)
        else:
            SENSOR2_PIN.value(False)
        if (lf > lfrontwallseen) and (rf > rfrontwallseen):
            pixels[0] = (0, 0, 64) # set to blue, 1/4 brightness
            pixels.write()
            LED_PIN.value(True)
        else:
            pixels[0] = (0, 0, 0) # set pixels off
            pixels.write()
            LED_PIN.value(False)
        print("ls",(ls),"lf",(lf),"rf",(rf),"rs",(rs),"  ", (leftside),(leftfront),(rightfront),(rightside))
        time.sleep(.5)
        time.sleep(.005)
    return

def progdisp():
    global swvalue, progno
    switchread()
    if(swvalue == 1):
        progno = progno + 1
        time.sleep(.5) 
    p1count = progno
    progno = p1count - (int(p1count / 8) * 8) # this gets it in range 0 to 7
    bit2 = p1count - (int(p1count / 2) * 2) # this extracts bit 3
    p1count = int(p1count / 2)
    bit1 = p1count - (int(p1count / 2) * 2) # this extracts bit 2
    p1count = int(p1count / 2)
    bit0 = p1count - (int(p1count / 2) * 2) # this extracts bit 1
    p1count = int(p1count / 2)
    if (bit0 == 1):
        SENSOR1_PIN.value(True)
    else:
        SENSOR1_PIN.value(False)
    if (bit1 == 1):
        SENSOR2_PIN.value(True)
    else:
        SENSOR2_PIN.value(False)
    if (bit2 == 1):
        LED_PIN.value(True)
    else:
        LED_PIN.value(False) 

def rightcount1():
    global r1count
    r1count = r1count+1
    return
def rightcount2():
    global r2count
    r2count = r2count+1
    return

def setoutsidewalls():
    for x in range(WIDTH):    # does range 0 to 15 when WIDTH = 16
        y = TABLEWIDTH * (HEIGHT - 1) + x
        walls[y] = walls[y] | NORTH    # set top (NORTH) walls
        walls[x] = walls[x] | SOUTH    # set bottom (SOUTH) walls
    for x in range(HEIGHT):
        y = (x * TABLEHEIGHT) + WIDTH - 1        
        walls[y] = walls[y] | EAST     # set right (EAST) walls      
        y = x * TABLEWIDTH
        walls[y] = walls[y] | WEST     # set left (WEST) walls
    # set wall to east of start cell    
    walls[0] = walls[0] | EAST
    walls[1] = walls[1] | WEST
    walls[0] = walls[0] | VISITED
    showwalls()
    
def setwalls():          # sets left, right and front walls seen when at start of cell boundary
    global ls, lf, rf, rs, heading, leftinit, rightinit, currentcell
    global lfrontwallseen, rfrontwallseen, leftwall, rightwall, frontwall
    if (debug == 1):
        print ("setwalls in cell", currentcell)
    leftwall = rightwall = frontwall = NOTPRESENT
    if (ls > (leftinit / 3)):
        leftwall = PRESENT
    if (rs > (rightinit / 3)):
        rightwall = PRESENT
    if ((lf > lfrontwallseen) and (rf > rfrontwallseen)):
        frontwall = PRESENT
    if (heading == NORTH):
        if (leftwall == PRESENT):
            walls[currentcell] = walls[currentcell] | WEST         # record left wall
            if (currentcell > 0):
                walls[currentcell - 1] = walls[currentcell - 1] | EAST  # record right wall in cell to left of current cell
        if (rightwall == PRESENT):
            walls[currentcell] = walls[currentcell] | EAST         # record right wall
            walls[currentcell + 1] = walls[currentcell + 1] | WEST # record left wall in cell to right of current cell
        if (frontwall == PRESENT):
            walls[currentcell] = walls[currentcell] | NORTH         # record front wall
            walls[currentcell + TABLEWIDTH] = walls[currentcell + TABLEWIDTH] | SOUTH  
    if (heading == SOUTH):
        if (leftwall == PRESENT):
            #print (currentcell)
            walls[currentcell] = walls[currentcell] | EAST         # record left wall
            walls[currentcell + 1] = walls[currentcell + 1] | WEST  # record right wall in cell to left of current cell
        if (rightwall == PRESENT):
            walls[currentcell] = walls[currentcell] | WEST         # record right wall
            walls[currentcell - 1] = walls[currentcell - 1] | EAST # record left wall in cell to right of current cell
        if (frontwall == PRESENT):
            walls[currentcell] = walls[currentcell] | SOUTH         # record front wall
            if(currentcell > TABLEWIDTH):
                walls[currentcell - TABLEWIDTH] = walls[currentcell - TABLEWIDTH] | NORTH           
    if (heading == EAST):
        if (leftwall == PRESENT):
            #print (currentcell)
            walls[currentcell] = walls[currentcell] | NORTH         # record left wall
            walls[currentcell + TABLEWIDTH] = walls[currentcell + TABLEWIDTH] | SOUTH  # record right wall in cell to left of current cell
        if (rightwall == PRESENT):
            walls[currentcell] = walls[currentcell] | SOUTH         # record right wall
            if(currentcell >= TABLEWIDTH):
                walls[currentcell - TABLEWIDTH] = walls[currentcell - TABLEWIDTH] | NORTH # record left wall in cell to right of current cell          
        if (frontwall == PRESENT):
            walls[currentcell] = walls[currentcell] | EAST         # record front wall
            walls[currentcell + 1] = walls[currentcell + 1] | WEST  
    if (heading == WEST):
        if (leftwall == PRESENT): 
            #print (currentcell)
            walls[currentcell] = walls[currentcell] | SOUTH         # record left wall
            if (currentcell >= 0):
                walls[currentcell - TABLEWIDTH] = walls[currentcell - TABLEWIDTH] | NORTH  # record right wall in cell to left of current cell
        if (rightwall == PRESENT):
            walls[currentcell] = walls[currentcell] | NORTH         # record right wall
            walls[currentcell + TABLEWIDTH] = walls[currentcell + TABLEWIDTH] | SOUTH # record left wall in cell to right of current cell           
        if (frontwall == PRESENT):
            walls[currentcell] = walls[currentcell] | WEST         # record front wall
            walls[currentcell - 1] = walls[currentcell - 1] | EAST  
    walls[currentcell] = walls[currentcell] | VISITED         # mark cell as visited after putting in the walls seen
 
def showflood():
    x = ((HEIGHT-1) * TABLEWIDTH)
    y = HEIGHT - 1
    if (debug == 1):
        print("Flood table")
        while (x >= 0):
            print (y," ", maze[x],maze[x+1],maze[x+2],maze[x+3],maze[x+4],maze[x+5],maze[x+6],maze[x+7],maze[x+8],maze[x+9],maze[x+10],maze[x+11],maze[x+12],maze[x+13],maze[x+14],maze[x+15])
            x = x - TABLEWIDTH
            y = y - 1
            
def showLEDwalls():
    global ls,rs,lf,rf,leftinit,rightinit,lfrontwallseen, rfrontwallseen
    # put LEDs on if walls seen
    if (ls > (leftinit / 3)):
        SENSOR1_PIN.value(True)
    else:
        SENSOR1_PIN.value(False)
    if (rs > (rightinit / 3)):
        SENSOR2_PIN.value(True)
    else:
        SENSOR2_PIN.value(False)    
    if  ((lf > lfrontwallseen) and (rf > rfrontwallseen)): # see front wall with both sensors
        LED_PIN.value(True)
    else:
        LED_PIN.value(False)

def showwalls():
    x = ((HEIGHT-1) * TABLEWIDTH) 
    y = HEIGHT - 1
    if (debug == 1):
        print("Maze walls")
        while (x >= 0):
            print (y," ", walls[x],walls[x+1],walls[x+2],walls[x+3],walls[x+4],walls[x+5],walls[x+6],walls[x+7],walls[x+8],walls[x+9],walls[x+10],walls[x+11],walls[x+12],walls[x+13],walls[x+14],walls[x+15])
            x = x - TABLEWIDTH
            y = y - 1


def stop():
    LMOTOR_PWM.duty_u16(0)
    RMOTOR_PWM.duty_u16(0)
    

def switchread():
    global swvalue
    swvalue = Switch.value() # = 0, or 1 if button pressed 

    
def switchtest():
    while True:
        global swvalue
        switchread()    
        print(("switch"),(swvalue))
        time.sleep(.5) # white around 4,500, black around 55,000
        
def turnaround(fwdfirst):
    global heading, currentcell
    global l1count, l2count, r1count, r2count, currdist, leftwall, rightwall, frontwall, leftinit, rightinit, ls , rs
    global leftspeed, rightspeed, basespeed, debug, speedbias
    if (debug == 1):
        print ("turnaround heading", heading, "currentcell", currentcell)
    if (fwdfirst > 0):
        goahead (fwdfirst, basespeed) # go forward to requested amount while following walls
    # stop motors, wait a fraction of a second
    leftspeed = 0
    rightspeed = 0
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    photoread()
    time.sleep_ms(100)
    if(debug == 1): # pause if in debug mode
        pause()
        print ("spin round 180 degrees")
    if ((lf > lfrontwallseen) and (rf > rfrontwallseen)): # checking for wall ahead before turn round
        backupok = YES
    else:
        backupok = NO
    # spin round on the spot 180 degrees
    encoderreset()       # reset encoder counts to zero
    encsum = l1count + l2count + r1count + r2count
    RMOTOR_DIR.value(0)  # set the right motor direction to reverse
    leftspeed = basespeed
    rightspeed = basespeed
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    gonesum = l1count + l2count + r1count + r2count  # number of encoder counts at start of turn
    dist = 2800
    # make sure right motor is turning at exactly the same amount aa the left one for distance required 
    turnfactor = 5 # amount to increase speed of motors if not turning at proper speeds
    while (gonesum < dist):
        leftencs = l1count + l2count  # sum of left encoder counts 
        rightencs = r1count + r2count # sum of right encoder counts
        turnerr = (leftencs - rightencs) * turnfactor
        lspeed = leftspeed + turnerr
        rspeed = rightspeed - turnerr
        checkspeed()
        LMOTOR_PWM.duty_u16(lspeed)
        RMOTOR_PWM.duty_u16(rspeed)
        gonesum = l1count + l2count + r1count + r2count  # number of encoder counts since start of turn
        time.sleep(0.001)
    leftspeed = 0
    rightspeed = 0
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    if(debug == 1): # pause if in debug mode
        pause()
    RMOTOR_DIR.value(1)  # set the right motor direction back to forward
    if(debug == 1): # pause if in debug mode
        pause()
        print("backup", backupok)
    if(backupok == YES): # is there a wall to back up to?
        backup()   # back up to rear wall and then set for forward
    headin = heading
    if (headin == NORTH):
        heading = SOUTH
    if (headin == EAST):
        heading = WEST
    if (headin == WEST):
        heading = EAST
    if (headin == SOUTH):
        heading = NORTH
    if(debug == 1): # 
        print("backed up. Heading is now", heading)
    photoread()   # check presence of walls on either side after turn round
    if (ls > (leftinit / 3)):
        leftwall = PRESENT
    else:
        leftwall = NOTPRESENT        
    if (rs > (rightinit / 3)):
        rightwall = PRESENT
    else:
        rightwall = NOTPRESENT  
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    # go forward to cell boundary checking not too close to either side wall and adjust if needed
    goahead(3000, basespeed)
    if(debug == 1): # pause if in debug mode
        pause()
    if (heading == NORTH):
        currentcell = currentcell + TABLEWIDTH # update cell pointer as we move into next cell
    if (heading == SOUTH):
        currentcell = currentcell - TABLEWIDTH # update cell pointer as we move into next cell
    if (heading == EAST):
        currentcell = currentcell + 1 # update cell pointer as we move into next cell
    if (heading == WEST):
        currentcell = currentcell - 1 # update cell pointer as we move into next cell
    
    return

def turnleft():
    global heading, basespeed, currentcell, debug, progno
    global l1count, l2count, r1count, r2count, currdist, leftwall, rightwall, frontwall, leftinit, rightinit, ls , rs
    global leftspeed, rightspeed, basespeed, debug, speedbias
    startcount = r1count + r2count # vaue of right counts when we call this procedure
    if (debug == 1):
        print ("turnleft in cell no", currentcell)
        SENSOR1_PIN.value(True)
        SENSOR2_PIN.value(False)
        LED_PIN.value(False)
        # halt()
    
    SENSOR1_PIN.value(False) # switch off left wall seen LED
    leftspeed = int(basespeed / 3)
    rightspeed = int(basespeed)
    checkspeed()
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    gonesum = 0
    goahead (500, basespeed)
    leftspeed = int(basespeed / 3)
    rightspeed = int(basespeed)
    checkspeed()
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    gonesum = 0
    dist = 2000
    encoderreset() # reset encoder counts 
    # make sure right motor is turning at exactly 3 times the left one for distance required
    turnfactor = 5 # amount to increase speed of motors if not turning at proper speeds
    while (gonesum < dist):
        leftencs = l1count + l2count * 3 # sum of left encoder counts times 3
        rightencs = (r1count + r2count) # sum of right encoder counts times 3
        turnerr = (leftencs - rightencs) * turnfactor
        lspeed = leftspeed + turnerr
        rspeed = rightspeed - turnerr
        checkspeed()
        LMOTOR_PWM.duty_u16(lspeed)
        RMOTOR_PWM.duty_u16(rspeed)
        gonesum = rightencs  # number of left encoder counts since start of turn
        time.sleep(0.001)
    if (debug == 1):
        pause()
    goahead (500, basespeed)    
    # update cell number 
    if (heading == NORTH):
        currentcell = currentcell - 1 # update cell pointer as we move into next cell
    if (heading == SOUTH):
        currentcell = currentcell + 1 # update cell pointer as we move into next cell
    if (heading == EAST):
        currentcell = currentcell + TABLEWIDTH # update cell pointer as we move into next cell
    if (heading == WEST):
        currentcell = currentcell - TABLEWIDTH # update cell pointer as we move into next cell
     # update heading
    headin = heading
    if (headin == NORTH):
        heading = WEST
    if (headin == EAST):
        heading = NORTH
    if (headin == WEST):
        heading = SOUTH
    if (headin == SOUTH):
        heading = EAST
    if (debug == 1):
        pause()   
    return
    
def turnright():
    global heading, basespeed, currentcell, debug, progno
    global l1count, l2count, r1count, r2count, currdist, leftwall, rightwall, frontwall, leftinit, rightinit, ls , rs
    global leftspeed, rightspeed, basespeed, debug, speedbias
    startcount = l1count + l2count # vaue of left encoder counts when we call this procedure
    if (debug == 1):
        print ("turnright in cell no", currentcell)
        SENSOR1_PIN.value(False)
        SENSOR2_PIN.value(True)
        LED_PIN.value(False)
        # halt()
    goahead (500, basespeed)    
    leftspeed = int(basespeed)
    rightspeed = int(basespeed / 3)
    checkspeed()
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    gonesum = 0
    dist = 2000
    encoderreset() # reset encoder counts 
    # make sure left motor is turning at exactly 3 times the right one for distance required
    turnfactor = 5 # amount to increase speed of motors if not turning at proper speeds
    while (gonesum < dist):
        leftencs = l1count + l2count # sum of left encoder counts
        rightencs = (r1count + r2count) * 3 # sum of right encoder counts times 3
        turnerr = (leftencs - rightencs) * turnfactor
        lspeed = leftspeed - turnerr
        rspeed = rightspeed + turnerr
        checkspeed()
        LMOTOR_PWM.duty_u16(lspeed)
        RMOTOR_PWM.duty_u16(rspeed)
        gonesum = leftencs  # number of left encoder counts since start of turn
        time.sleep(0.001)
    leftspeed = (basespeed)
    rightspeed = (basespeed)
    checkspeed()
    LMOTOR_PWM.duty_u16(leftspeed)
    RMOTOR_PWM.duty_u16(rightspeed)
    if (debug == 1):
        pause()
    goahead (500, basespeed)    
    #  change current cell number
    if (heading == NORTH):
        currentcell = currentcell + 1 # update cell pointer as we move into next cell
    if (heading == SOUTH):
        currentcell = currentcell - 1 # update cell pointer as we move into next cell
    if (heading == EAST):
        currentcell = currentcell - TABLEWIDTH # update cell pointer as we move into next cell
    if (heading == WEST):
        currentcell = currentcell + TABLEWIDTH # update cell pointer as we move into next cell
    #  change heading
    headin = heading
    if (headin == NORTH):
        heading = EAST
    if (headin == EAST):
        heading = SOUTH
    if (headin == WEST):
        heading = NORTH
    if (headin == SOUTH):
        heading = WEST
    if (debug == 1):    
        pause()
    return


def walltest():
    global basespeed, currdist, leftinit, rightinit, ls, rs
    calibrate()
    photoread()
    
    currdist = 0
    basespeed = 15000
    goahead (20000, 15000)
    halt()
    

def wherenext():      # decide whether to go ahead or turn
    # Logic
    # get flood number for current cell we are moving into
    # If no wall to right get fllod number of wall to right
    # if lower than lowest so far set into lowest flood and save move needed to go to it
    # If no wall to the left get flood number for wall to the left
    # if left cell flood number is less than current and less than right cell number store cell number as next
    # If no wall ahead check flood number for cell ahead and if lower than others set that as where to go
    # if there were walls all round set previous cell as where to go and action is turn round
    # Note that we may be moving in any direction so right or left or ahead cells may be in any direction depending on the heading
    #
    global heading, prevcell, currentcell, maze, walls, move
    if (debug == 1):
        print ("wherenext heading", heading, "currentcell", currentcell)
    currflood = maze[currentcell]   # get flood number for cell we are moving in to
    lowestflood = currflood         # field to hold best flood number of adjacent cells
    
    if (heading == NORTH):
        nextcell = currentcell + TABLEWIDTH              # next cell is one above
        if (walls[currentcell] & NORTH == 0):            # check if no wall ahead
            if (maze[nextcell] < lowestflood):           # see if lower flood in that cell
                lowestflood = maze[nextcell]             # if so, save lowest flood no
                move = AHEAD                             # save move we need to make
        nextcell = currentcell + 1                       # next cell is one to right
        if (walls[currentcell] & EAST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = RIGHT
        nextcell = currentcell - 1                       # next cell is one to left
        if (walls[currentcell] & WEST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = LEFT
        nextcell = currentcell - TABLEWIDTH                   # next cell is behind us 
        if (walls[currentcell] & SOUTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = ROUND
                
    if (heading == SOUTH):
        nextcell = currentcell - TABLEWIDTH                   # next cell is one below us
        if (walls[currentcell] & SOUTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = AHEAD
        nextcell = currentcell + 1                       # next cell is one to the left
        if (walls[currentcell] & EAST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = LEFT
        nextcell = currentcell - 1                       # next cell is one to the right
        if (walls[currentcell] & WEST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = RIGHT
        nextcell = currentcell + TABLEWIDTH                   # next cell is behind us
        if (walls[currentcell] & NORTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = ROUND  
    
    if (heading == EAST):
        nextcell = currentcell + 1                        # next cell is ahead
        if (walls[currentcell] & EAST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = AHEAD
        nextcell = currentcell + TABLEWIDTH                   # next cell is one to the left
        if (walls[currentcell] & NORTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = LEFT
        nextcell = currentcell - TABLEWIDTH                    # next cell is one to the right
        if (walls[currentcell] & SOUTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = RIGHT
        nextcell = currentcell - 1                         # next cell is one behind us
        if (walls[currentcell] & WEST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = ROUND  

    if (heading == WEST):
        nextcell = currentcell - 1                        # next cell is ahead
        if (walls[currentcell] & WEST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = AHEAD
        nextcell = currentcell - TABLEWIDTH                   # next cell is one to the left
        if (walls[currentcell] & SOUTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = LEFT
        nextcell = currentcell + TABLEWIDTH                    # next cell is one to the right
        if (walls[currentcell] & NORTH == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = RIGHT
        nextcell = currentcell + 1                         # next cell is one behind us
        if (walls[currentcell] & EAST == 0):
            if (maze[nextcell] < lowestflood):
                lowestflood = maze[nextcell]
                move = ROUND  
    return


def mazesolve():
    global leftfront, rightfront, leftspeed, rightspeed, heading, prevcell, currentcell, currdist
    global basespeed, leftwall, rightwall, frontwall, move
    basespeed = 15000    # standard running speed
    currentspeed = 0    # expected current speed
    leftspeed = basespeed
    rightspeed = basespeed
    side = 0
    adjustment = 0
    prevdiff = 0
    currentcell = 0     # number of the cell we are in
    heading = NORTH     # direction the mouse is pointing
    state = EXPLORE     # state that is controlling what we do
    route = OUT          # go - OUT from start or BACK to start
    calibrate()           # align mouse in centre of the start cell
    leftwall = rightwall = PRESENT
    frontwall = NOTPRESENT
    movecount = 0
    if (debug == 1):
            print ("maze size", WIDTH, "x", HEIGHT, "Middle cell no", MIDDLE)
    while state == EXPLORE:     # move loop for explore
        movecount = movecount + 1
        if (debug == 1):
            print ("movecount at start", movecount)
        if (currentcell == 0):
            buttonwait()        # wait for start button pressed
            backup()            # reverse back to wall in start cell
            encoderreset()
            LMOTOR_PWM.duty_u16(leftspeed)  # reset to moving forward at basespeed
            RMOTOR_PWM.duty_u16(rightspeed) # reset to moving forward at basespeed
            currdist = 800 # record that we are starting 40 mm into the cell from the boundary
            goahead(3000,basespeed) # go 14mm to axles at 1st cell boundary. finish at basespeed
            prevcell = currentcell
            currentcell = TABLEWIDTH
            
        # main loop that is executed very time we go to a cell boundary
        photoread() # read the wall sensors at cell boundary
        setwalls() # set the left, right and front wall presence flags for the cell we are just going into
        showwalls()  # if in debug mode print out the walls
        if (route == OUT):
            floodmaze(MIDDLE,START) # flood from middle cell to start cell
        if (route == BACK):
            floodmaze(START, MIDDLE) # flood from start cell to middle cell
        if (floodfail == 1):
            print ("Flood failed", MIDDLE, START)
            showflood()
            showwalls()
            beep() # beep  3 times to say flood failed
            beep()
            beep()
            halt() # then halt
        prevcell = currentcell
        wherenext()  # get direction that we want to go in move field
        if (debug == 1):
            print ("Cell", currentcell, "Heading", heading, "Move", move) 
        if (move == AHEAD):
            currdist = 0 # starting at beginning of cell
            goonecellahead()
        if (move == RIGHT):
            turnright()
        if (move == LEFT):
            turnleft()
        if (move == ROUND):
            turnaround(2100) 
        if (debug == 1):
            showflood()
        print ("movecount", movecount, "done", "currentcell", currentcell)
        
        # This section determines what we do when we get to the boundary of the centre cell
        if ((route == OUT) and (currentcell == MIDDLE)):  # Reached middle cell
            photoread()
            setwalls() # set the left, right and front wall presence flags for the cell we are just going into
            showLEDwalls() # set LEDs on to show the walls
            goahead(2000,basespeed) # move to centre of middle/end cell
            stop()    # stop motors
            beep()    # beep to say reached midddle cell
            pause()   # wait 2 seconds
            if ((lf > lfrontwallseen) and (rf > rfrontwallseen)):
                turnaround(0)
            else:
                goahead(2100,basespeed) # move to edge of middle/end cell
                # update cell number
                if (heading == NORTH):
                   currentcell  = currentcell + TABLEWIDTH
                if (heading == SOUTH):
                    currentcell  = currentcell - TABLEWIDTH
                if (heading == EAST):
                   currentcell  = currentcell + 1   
                if (heading == WEST):
                   currentcell  = currentcell - 1  
            route = BACK   # reset route direction after reached middle/end cell
            
        if ((route == BACK) and (currentcell == START)):  # Reached start cell again
            goahead(2100,basespeed) # move to middle of start cell
            pause()
            beep()    # beep to say reached midddle cell
            if (debug == 1):
                print ("end of explore phase currentcell ", currentcell)
            turnaround(0) # and backup then go to first cell boundary
            route = OUT # reset to head towards the centre
            # 
          
    # should not get here
    showwalls()
    stop()
    beep()    # beep 
    halt()    

# maze solver routines needed:
# calcroute - build a list of cells to get from 1st cell to 2nd cell using flood info
# calibrate - get mouse aligned centrally between the walls
# detectwalls - read sensors and set flags to say which walls seen if any
# floodmaze - flood maze from 1st cell number to 2nd cell number
# moveahead - go specified distance forward and at specified end speed whilst following walls if present
# runroute - run a sequence of cells at a given speed
# setaheadwall - set flag for wall seen ahead when at centre of cell
# setsidewalls  set flags for the side walls seen as we go into that cell
# triggerstart - detect pulse in front of mouse o start it once against back wall
# turnleft - smooth turn left from middle of cell to middle of cell
# turnright - smooth turn right from middle of cell to middle of cell
# turnaround  - stop and then spin 180 degrees, and if present back up to rear wall after spin
# wherenext - decide when in cell centre whether to continue ahead or turn

        # Logic for explore to centre
        # start with axles at cell boundary        
        # Read and set Left & Right Walls
        # do flood, then choose adjacent cell with lowest flood number (Left,Right or Ahead) and no wall in the way
        # If left or right cell is lowest do left or right turn to cell boundary
        # if ahead is lowest, move to centre of cell and check if wall ahead. Set wall ahead if it is there.
        # If no wall ahead go forward to cell boundary
        # if wall ahead, stop in centre then spin turn to lowest flood number cell at side or back.
        #    then go to cell boundary
        # Update current cell number
        # Go round loop again



    
    
    
# ------- End of definitions -----------------------------------------------------
#---------------------------------------------------------------------------------
# Melody
MELODY_NOTE = [659, 659, 0, 659, 0, 523, 659, 0, 784]
MELODY_DURATION = [0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.2]

#-------------------------------------------------

LED_PIN.value(1)     # switch on LED on PCB
SENSOR1_PIN.value(1) # switch on sensor1 LED on sensor board
SENSOR2_PIN.value(1) # switch on sensor2 LED on sensor board

RMOTOR_DIR.value(1)  # set the right motor direction
LMOTOR_DIR.value(0)  # set the left motor direction

# Initialize Neopixel RGB LEDs
pixels = neopixel.NeoPixel(machine.Pin(11), 2)
pixels[0] = (64, 0, 0) # set to red, 1/4 brightness
pixels[1] = (0, 128, 0) # set to green, half brightness
pixels.write()
time.sleep(1)
pixels[0] = (0, 0, 64) # set to blue, 1/4 brightness
pixels[1] = (64, 0, 0) # set to red
pixels.write()
time.sleep(1)
pixels.fill((0, 0, 0)) # clear neopixels
pixels.write()

beep()

# Play tone
PIEZO_PWM = machine.PWM(machine.Pin(22))
PIEZO_PWM.freq(440)
PIEZO_PWM.duty_u16(30000) # loudness in range 0 to 65535
time.sleep(0.5)
PIEZO_PWM.duty_u16(0)


l1count = l2count = r1count = r2count = 0 #reset the encoder counts
left1 = left2 = right1 = right2 = True
prevleft1 = prevleft2 = prevright1 = prevright2 = True # reset the previous encoder values
progno = 0
swvalue = 0
heading = 1


# configure irq callback for encoders
Leftenc1.irq(lambda p:leftcount1())
Leftenc2.irq(lambda p:leftcount2())
Rightenc1.irq(lambda p:rightcount1())
Rightenc2.irq(lambda p:rightcount2())

# define size of maze cells - extra row added at top and one to right for processing at edge of maze

numcells = (TABLEWIDTH * (TABLEHEIGHT + 1)) + 10

# set up and initialise arrays for walls and flood values in maze
walls = array.array('i',[0]*numcells) # array of cell items initialised to zero starting at cell 0
#walls[0] = 99   example of setting an indexed value 
maze = array.array('i',[0]*numcells) # array that holds flood values for maze cells
# cells are numbers from left to right then upwards from start cell as zero
# to check bits in a byte use walls[n] & NORTH to check the north wall bit, walls[n] & EAST for next bit etc
proclist = array.array('i',[0]*numcells) # array that holds the list of cells to be processed next by teh flood routine
setoutsidewalls()


#flashLEDs()  # test if LEDs on sensor board are working

# -------------------------------------------------
# wait for GP20 onboard button to be pressed
setting = 1
while (setting == True): # Check button 1 (GP20)
    setting = btn1.value()
    #print ("progno", progno)
    progdisp()
    #encountdisp()     
    time.sleep(0.1)
print ("progno", progno)
    
# run selected program
debug = 0             # if debug is set  to 1 it will display debugging info
if (progno == 1):
    mazesolve()
if (progno == 2):
    debug = 1
    mazesolve()
if (progno == 3):
    phototest()
if (progno == 4):
    encodertest()
if (progno == 5):    
    switchtest()
if (progno == 6):    
    motortest()
if (progno == 7):    
    walltest()

SENSOR1_PIN.value(0) # switch off sensor1 LED on wall folllower board
SENSOR2_PIN.value(0) # switch off sensor2 LED on wall folllower board
LMOTOR_PWM.duty_u16(0) # stop L motor - range is 0 to 65535
RMOTOR_PWM.duty_u16(0) # stop R motor - range is 0 to 65535