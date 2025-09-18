import pin_bot
from machine import Pin
import machine
import time, funs

temp_dis = 0

a_count = 0
a_check = 0
a_abs = 0

b_count = 0
b_check = 0
b_abs = 0

def reset():
    global a_count,b_count,a_check,b_check
    a_count = 0
    b_count = 0
    a_abs = 0
    b_abs = 0
    #a_check = 0
    #b_check = 0
    
def read_encoder_a(pin):
    global a_count,a_check,a_abs,b_abs
    
    if pin_bot.ea2.value():  # Kiểm tra pha B để xác định hướng
        a_count -= 1  # Quay ngược
        a_abs +=1
    else:
        a_count += 1  # Quay xuôi
        a_abs +=1
# Hướng quay của encoder B
def read_encoder_b(pin):
    
    global b_count,b_check,a_abs,b_abs
    
    if pin_bot.eb2.value():  # Kiểm tra pha B để xác định hướng
        b_count -= 1  # Quay ngược
        b_abs +=1
    else:
        b_count += 1
        b_abs +=1

#460/1cell
#120/-120/ 90'

pin_bot.ea1.irq(trigger=Pin.IRQ_RISING, handler=read_encoder_a)
pin_bot.eb1.irq(trigger=Pin.IRQ_RISING, handler=read_encoder_b)

def A_speed_rp():
    irq_state = machine.disable_irq()  # Tắt ngắt để tránh lỗi đọc
    speed = abs(a_count)
    machine.enable_irq(irq_state)  # Bật lại ngắt
    return speed

def B_speed_rp():
    irq_state = machine.disable_irq()
    speed = abs(b_count)
    machine.enable_irq(irq_state)
    return speed

def dict_rp():
    global a_count,b_count
    return a_count+b_count
def speed_init():
    global temp_dis,a_check,b_check
    temp_dis +=abs(a_count)+abs(b_count)
    a_check +=a_count
    b_check +=b_count
    reset()
    time.sleep(0.01)
def reset_abs():
    global a_abs,b_abs
    a_abs,b_abs = 0,0
def abs_ab():
    global a_abs,b_abs
    return a_abs + b_abs