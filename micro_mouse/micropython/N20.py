import machine
import time
import pin_bot
import encoder
import PID ,ir_sensor, funs
from ir_sensor import adc ,read_all,is_wall_ahead ,is_wall_right,is_wall_left

time_chance = 10000
b = 1.
a = 1.
k_temp = 400
base_speed   = 40000
base_dict = 18
base_speed_A = 40000
balance = 2500
base_speed_B = base_speed_A-balance

left_distance =  23000
right_distance = 37000
        
def begin():
    #pin_bot.Stby.off()
    pin_bot.Stby.on()
begin()
def stop():
    control_A(-1000)
    control_B(-12000)
    time.sleep(0.01)
    pin_bot.Ain1.off()
    pin_bot.Ain2.off()
    pin_bot.PwmA.duty_u16(0)
    pin_bot.Bin1.off()
    pin_bot.Bin2.off()
    pin_bot.PwmB.duty_u16(0)
    
    
def control_A(speed):
    pin_bot.PwmA.duty_u16(min(abs(int(a*speed)),65535))
    if speed > 0.:
        pin_bot.Ain1.on()
        pin_bot.Ain2.off()
    if speed < 0.:
        pin_bot.Ain1.off()
        pin_bot.Ain2.on()
    if speed==0:
        pin_bot.Ain1.off()
        pin_bot.Ain2.off()
              
def control_B(speed):
    pin_bot.PwmB.duty_u16(min(abs(int(b*speed)),65535))
    if speed > 0.:
        pin_bot.Bin1.on()
        pin_bot.Bin2.off()
    if speed < 0.:
        pin_bot.Bin1.off()
        pin_bot.Bin2.on()
    if speed==0:
        pin_bot.Bin1.off()
        pin_bot.Bin2.off()
        
def control_wheel(a,b):
    
    control_A(a)
    control_B(b)

def run_init():
    begin()
    ir_sensor.read_all()
    

def go_ahead(dict,set_speed):
    global base_speed, base_speed_A, base_speed_B,k_temp ,base_dict,balance
    encoder.temp_dis = 0
    encoder.speed_init()
    encoder.reset()
    xd =250
    temp_speed_A = base_speed_A
    temp_speed_B = base_speed_B
    
    while encoder.temp_dis < dict :
        
        encoder.speed_init()
        read_all()
        
        control_wheel(temp_speed_A,temp_speed_B)
    
        
        if is_wall_right() == 0 and is_wall_left() == 0:
            control_wheel(base_speed_A,base_speed_B)
        
        if is_wall_right() == 1 and adc[1].value > right_distance+1000  :
            temp_speed_A+= xd
            temp_speed_B -=xd
        elif is_wall_right() ==1 and adc[1].value < right_distance :
            temp_speed_A-=xd
            temp_speed_B+=xd
        
        if is_wall_left() ==1 and adc[2].value > left_distance+ 1000  :
            temp_speed_A -=xd
            temp_speed_B+= xd
        elif is_wall_left() ==1 and adc[2].value < left_distance :
            temp_speed_A+=xd
            temp_speed_B-=xd
        
        if adc[1].value > right_distance+5000  and adc[2].value > left_distance+5000:
            control_wheel(base_speed_A,base_speed_B)
    balance = temp_speed_A - temp_speed_B
    base_speed_A = 40000
    base_speed_B = base_speed_A-balance
    
dict_go_ahead = 2060
def go_one_cell():
    global dict_go_ahead
    go_ahead(dict_go_ahead,base_dict)
    
turn_left_long_dis = 1620
turn_left_th2_dis = 930
def turn_to_left():
    global base_speed, base_speed_A, base_speed_B,k_temp ,base_dict,turn_left_long_dis,turn_left_th2_dis
    long_dis = turn_left_long_dis
    th1_dis = 10
    th2_dis = turn_left_th2_dis
    
    encoder.reset()
    encoder.temp_dis = 0
    tempA = base_speed_A
    tempB = base_speed_B
    while encoder.temp_dis < long_dis :
        
        encoder.speed_init()
        
        if encoder.temp_dis >th1_dis and encoder.temp_dis < th2_dis :
            control_wheel(tempA,4000)
        else :
            control_wheel(tempA,tempB)
        if encoder.A_speed_rp() > base_dict :
            tempA -= k_temp
        elif encoder.A_speed_rp() < base_dict :
            tempA += k_temp
        
        if encoder.B_speed_rp() > base_dict :
            tempB -= k_temp
        elif encoder.B_speed_rp() < base_dict :
            tempB += k_temp
    
    
    stop()
    time.sleep(0.2)
    encoder.reset()
    

turn_right_long_dis = 1620
turn_right_th2_dis = 840
def turn_to_right():
        global base_speed, base_speed_A, base_speed_B,k_temp ,base_dict,turn_right_long_dis,turn_right_th2_dis
        long_dis = turn_right_long_dis
        th1_dis = 10
        turn_right_th2_dis = 880
        
        encoder.reset()
        encoder.temp_dis = 0
        tempA = base_speed_A
        tempB = base_speed_B
        while encoder.temp_dis < long_dis :
            
            encoder.speed_init()
            
            if encoder.temp_dis >th1_dis and encoder.temp_dis < turn_right_th2_dis :
                control_wheel(2000,tempB)
            else :
                control_wheel(tempA,tempB)
                
            if encoder.A_speed_rp() > base_dict :
                tempA -= k_temp
            elif encoder.A_speed_rp() < base_dict :
                tempA += k_temp
            
            if encoder.B_speed_rp() > base_dict :
                tempB -= k_temp
            elif encoder.B_speed_rp() < base_dict :
                tempB += k_temp
        
        stop()
        time.sleep(0.2)
        encoder.reset()
        
def round_back():
        
        global base_speed, base_speed_A, base_speed_B ,base_dict
        
        true_while = 1520
        k_temp = 2000
        encoder.reset_abs()
        encoder.reset()
        while(encoder.abs_ab()<true_while):
            if (encoder.a_abs <encoder.b_abs):
                control_wheel(base_speed_A+k_temp,-(base_speed_B))
            elif (encoder.a_abs >encoder.b_abs):
                control_wheel(base_speed_A,-(base_speed_B+k_temp))
            else:
                control_wheel(base_speed_A,-base_speed_B)
        stop()
        time.sleep(0.5)
        control_wheel(-base_speed_A-4000,-base_speed_B+4000)
        time.sleep(0.8)
        stop()
        time.sleep(1)
        encoder.reset()
        #go_one_cell()
        
        
def bam_phai():
    global base_speed_A,base_speed_B
    x = time.ticks_ms()
    while True:
        read_all()
        
        control_wheel(base_speed_A,base_speed_B)
        if time.ticks_ms() - x >time_chance:
            break
        
        if  is_wall_left() ==1 and is_wall_right() ==1 and  is_wall_ahead() == 1:
            control_wheel(base_speed_A,-base_speed_B)
            time.sleep(0.4)
        
        if is_wall_right() ==1 and is_wall_ahead() == 1:
            control_wheel(base_speed_A+25000,base_speed_B-20000)
            time.sleep(0.6)
            
        if is_wall_right() ==1 and adc[1].value > 32000  :
            control_wheel(base_speed_A+ 4000,base_speed_B -4000)
        elif is_wall_right() ==1 and adc[1].value < 30000:
            control_wheel(base_speed_A-4000,base_speed_B+4000)
        elif is_wall_right() ==0:
            control_wheel(base_speed_A-15000,base_speed_B+10000)

def bam_trai():
    global base_speed_A,base_speed_B
    x = time.ticks_ms()
    while True:
        read_all()
        
        control_wheel(base_speed_A,base_speed_B)
        if time.ticks_ms() - x >time_chance:
            break
        
        if  is_wall_left() ==1 and is_wall_right() ==1 and  is_wall_ahead() == 1:
            control_wheel(-base_speed_A,base_speed_B)
            time.sleep(0.4)
            
        if is_wall_left() ==1 and  is_wall_ahead() == 1:
            control_wheel(base_speed_A-20000,base_speed_B +25000)
            time.sleep(0.6)
            
        if is_wall_left() ==1 and adc[2].value > 27000  :
            control_wheel(base_speed_A -4000,base_speed_B+ 4000)
        elif is_wall_left() ==1 and adc[2].value < 25000:
            control_wheel(base_speed_A+4000,base_speed_B-4000)
        elif is_wall_left() ==0:
            control_wheel(base_speed_A+10000,base_speed_B-15000)
