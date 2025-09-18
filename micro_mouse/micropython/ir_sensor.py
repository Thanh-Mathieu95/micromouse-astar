import pin_bot,time
from vlx_laser import setup_tofl_device, TBOOT
import utime

k = 0

wall_right = 32000
wall_left = 15000
ahead1 = 8000
ahead2 = 1000

class Sensor :
    
    def __init__(self, adc):
        self.adc = adc
        self.value = 0
        self.temp_value = 0
        self.min_wall = 8000
        self.x_wall = 0
    
    def read_adc(self):
        x = pin_bot.adc_pins[self.adc].read_u16()
        pin_bot.ir_pin.duty_u16(65000)
        pin_bot.LED_PIN.on()
        time.sleep_ms(50)
        y = pin_bot.adc_pins[self.adc].read_u16()
        pin_bot.ir_pin.duty_u16(000)
        pin_bot.LED_PIN.off()
        time.sleep_ms(50)
        
        return y - x
    
    def wall_bool(self):
        
        if self.value > self.min_wall :
            
            return True
        else:
            return False
        
    def update(self):
       
        self.temp_value = self.value

adc = [Sensor(0), Sensor(1), Sensor(2), Sensor(3)]

adc[1].min_wall = wall_right

adc[2].min_wall = wall_left

adc[0].min_wall = ahead1

adc[3].min_wall = ahead2

def distance_init():
    global left,right
    read_all()
    left = adc[1].value
    right = adc[2].value

def read_all():
    global adc , left,right
    for i in range(4):
        adc[i].value = pin_bot.adc_pins[i].read_u16()
        
    pin_bot.ir_pin.on()
    time.sleep_ms(20)
    
    for i in range(4):
        adc[i].value = pin_bot.adc_pins[i].read_u16() - adc[i].value
        if adc[i].value == 0:
            adc[i].value = 0
    
    pin_bot.ir_pin.off()
    time.sleep_ms(20)
    
    left = adc[2].value
    right = adc[1].value



def is_wall_ahead():
    global adc
    if adc[0].wall_bool() and adc[3].wall_bool():
        return 1
    elif adc[0].wall_bool() or adc[3].wall_bool():
        read_all()
        if adc[0].wall_bool() and adc[3].wall_bool():
            return 1
        else:
            return 0
    else :
        return 0


def is_wall_left():
    global adc
    if adc[2].wall_bool():
        return 1
    else :
        return 0
    
def is_wall_right():
    global adc
    if adc[0].value >15000:
        adc[1].value -= 5000
    
    if adc[1].wall_bool():
        return 1
    else :
        return 0
