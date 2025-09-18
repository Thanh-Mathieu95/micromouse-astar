import pin_bot
import time
import machine
coutB1 = 0
coutB2 = 0
value_temp = 0
def reset():
    global coutB1, coutB2
    coutB1 = 0
    coutB2 = 0
def get_buttonA(pin):
    global coutB1
    coutB1 += 1
    #time.sleep(0.5)
def get_buttonB(pin):
    global coutB2
    coutB2 += 1
    #time.sleep(0.5)

def wait():
    reset()
    while not coutB1:
        pin_bot.LED_PIN.off()
    machine.reset()
        
def beep(freq, duration):
    pin_bot.LED_PIN.on()
    pin_bot.Buzzer.freq(int(10*freq))
    pin_bot.Buzzer.duty_u16(32768)  # t% chu kỳ làm việc
    time.sleep_ms(int(duration*1.2))
    pin_bot.Buzzer.duty_u16(0)
    pin_bot.LED_PIN.off()# Tắt pin_bot.Buzzer

def levelUp():
    beep(329, 100)  # NOTE_E4
    beep(392, 100)  # NOTE_G4
    beep(659, 100)  # NOTE_E5
    beep(523, 100)  # NOTE_C5
    beep(587, 100)  # NOTE_D5
    beep(784, 100)  # NOTE_G5

def note():
    t =70
    beep(400,200)
    pin_bot.LED_PIN.off()
    time.sleep_ms(int(1.5*t))
    beep(400,200)
    pin_bot.LED_PIN.off()
    time.sleep_ms(t)
    beep(200,200)
    pin_bot.LED_PIN.off()
    time.sleep_ms(t)
    beep(500,200)
    pin_bot.LED_PIN.off()
    time.sleep_ms(t)
    beep(200,200)
    pin_bot.LED_PIN.off()
    time.sleep_ms(int(1.5*t))
    beep(400,200)

pin_bot.BUTTON_1.irq(trigger=machine.Pin.IRQ_RISING, handler=get_buttonA)
pin_bot.BUTTON_2.irq(trigger=machine.Pin.IRQ_RISING, handler=get_buttonB)