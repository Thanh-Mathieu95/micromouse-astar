import machine,os
import time
import pin_bot
import ir_sensor
from ssd1306 import SSD1306_I2C
import encoder
import funs,N20
oled = SSD1306_I2C(128, 64, pin_bot.i2c)


def main_menu() :
    oled.fill(0)
    oled.text("Sunflower",22,0)
    oled.text("Search",0,13)
    oled.text("Run",0,26)
    oled.text("Setings",0,39)
    oled.text("Map",0,52)
    oled.rect(115, (funs.coutB2%4)*13 +12, 10, 10, 1)

def setting_menu():
    
    oled.fill(0)
    oled.text("Setings:",30,0)
    oled.text("Sensor",0,13)
    oled.text("Speed",0,26)
    oled.text("Move",0,39)
    oled.text("Back",0,50)
    oled.rect(115, (funs.coutB2%4)*13 +13, 10, 10, 1)
    oled.show()

def start_display():
    oled.fill(0)
    i=0
    while True:
        oled.fill_rect(0, 0, i, 64, 1)
        i+=10
        time.sleep_ms(50)
        oled.show()
        if i >= 128:
            oled.fill(0)
            oled.show()
            break
    
def map(lenh):
    oled.fill(0)
    x=0
    y=0
    for i in lenh:
        if x >110:
            y+=15
            x=0
        oled.text(str(i),x,y)
        
        x+=15
    oled.show()

def cam_bien():
    
    oled.fill(0)
    
    for i in range(4):
        oled.text(f"{ir_sensor.adc[i].value:.1f}", 0, 16*i +3)
    if ir_sensor.is_wall_ahead() ==1:
        oled.text("|",60,0 +3)
        oled.text("|",60,52)
            
    if ir_sensor.is_wall_left() == 1:
        oled.text("_",60,32)
    if ir_sensor.is_wall_right() == 1:
        oled.text("_",60,20 )
    
    oled.text("Fix",85,10)
    oled.rect(80, 0, 45, 30, 1)
    
    oled.text("Back",85,45)
    oled.rect(80, 34, 45, 30, 1)
    
    oled.fill_rect(120, (funs.coutB2%2)*34, 18, 30, 10)
    
    oled.show()

def toc_do():
    oled.fill(0)
    
    oled.rect(10,2, 30, 20, 10)
    #oled.text(f"{N20.a}",50,10)
    oled.rect(10,44, 30, 20, 10)
    #oled.text(f"{N20.b}",50,50)
    
    oled.fill_rect(10, (funs.coutB2%2)*42+2, 30, 20, 10)
    
    oled.show()

def cam_bien_fix(cam_bien,value):
   
    oled.fill(0)
    
    #oled.text(f"{cam_bien}",80,27)
    
    oled.rect(78, 10, 50, 50, 10)
    
    oled.text(f"{value}", 100,20,1)
    
    if cam_bien=="ahead":
        oled.fill_rect(74, 10, 3, 50, 10)
        
        oled.rect(110, 25, 20, 20, 10)
    
    if cam_bien=="right":
        oled.fill_rect(78, 6, 50, 3, 10)
        
        oled.rect(106, 22, 20, 20, 10)
        
    if cam_bien=="left":
        oled.fill_rect(78, 61, 50, 3, 10)
        
        oled.rect(106, 32, 20, 20, 10)
    
    for i in range(4):
        oled.text(f"{ir_sensor.adc[i].value:.1f}", 0, 16*i +3)
    if ir_sensor.is_wall_ahead() ==1:
        oled.text("|",60,0 +3)
        oled.text("|",60,52)
            
    if ir_sensor.is_wall_left() == 1:
        oled.text("_",60,32)
    if ir_sensor.is_wall_right() == 1:
        oled.text("_",60,20 )
        
    oled.show()
def move_fix():
    oled.fill(0)
    oled.text("Move fix:",30,0)
    oled.text("go_ahead",0,13)
    oled.text("turn right",0,26)
    oled.text("turn left",0,39)
    oled.text("Back",0,50)
    oled.rect(115, (funs.coutB2%4)*13 +13, 10, 10, 1)
    oled.show()

def goahead_fix(dict):
    oled.fill(0)
    oled.text(f"dict ahead :{dict}",0,15)
    oled.text("OK",56,46)
    oled.rect(24, 40, 80, 20, 1)
    oled.show()

def turn_right(turn_dict, go_dict):
    oled.fill(0)
    oled.text("R",110,46)
    oled.text(f"turn : {turn_dict}",0,6)
    oled.text(f"next : {go_dict}",0,22)
    oled.text("OK",56,46)
    oled.rect(24, 40, 80, 20, 1)
    oled.fill_rect(110,(funs.coutB2%2)*16+5,20,10,1)
    oled.show()
def turn_left(turn_dict, go_dict):
    oled.fill(0)
    oled.text("L",110,46)
    oled.text(f"turn : {turn_dict}",0,6)
    oled.text(f"next : {go_dict}",0,22)
    oled.text("OK",56,46)
    oled.rect(24, 40, 80, 20, 1)
    oled.fill_rect(110,(funs.coutB2%2)*16+5,20,10,1)
    oled.show()
while False:
    turn_right(1000,1600)
def read_file(file_path):
    oled.fill(0)
    try:
         with open(file_path, "r") as file:
            char_num = 0  # Đếm số thứ tự của ký tự
            i = 0
            while True:
                char = file.read(1)  # Đọc một ký tự tại một thời điểm
                if not char:  # Nếu không còn ký tự nào để đọc
                    break
                oled.text(f"{char}",10*char_num,i)
                oled.show()
                char_num += 1
                if char_num >=11 :
                    i +=15
                    char_num =0
    except FileNotFoundError:
        print(f"Lỗi: Không tìm thấy tệp {file_path}")
    except OSError as e:
        print(f"Lỗi khác: {e}")
def up_file():
    oled.fill(0)
    oled.text("Map", 40,0)
    files = os.listdir("map")
    nums = 10
    for file in files:
        oled.text(f"{file}", 0,nums)
        nums +=10
    oled.rect(115, (funs.coutB2%6)*10 , 9, 9, 1)
    oled.show()

def encoder_mode():
    oled.fill(0)
    oled.text(f"{encoder.a_count}", 0,20)
    oled.text(f"{encoder.b_count}", 0,40)
    oled.show()
    time.sleep(0.01)

def remove(value):
    oled.fill(0)
    oled.text(f"{value}: {N20.time_chance}", 0,20)
    oled.show()