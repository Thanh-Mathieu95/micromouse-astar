
import pin_bot, N20, encoder ,funs , time, machine,oled_display, PID ,ir_sensor,file,PID

from flood_fill import run_algorithm,initial_flood,maze_print
from robot import mouse
import data_init.data

main_run_map = "filenotf.txt"

while True :
    
    oled_display.main_menu()
    if main_run_map == "filenotf.txt":
        oled_display.oled.text("!",116,0)
    else:
        temp_screen = main_run_map
        temp_screen.replace('map/run_', '')
        temp_screen.replace('.txt', '')
        oled_display.oled.text(f"{temp_screen}",110,0)
    oled_display.oled.show()
    if funs.coutB1 >= 1 :
         
        funs.beep(2000,200)
         
        if funs.coutB2 % 4 == 0 :
            funs.reset()
            oled_display.start_display()
            break
            
        if funs.coutB2 % 4 == 1 :
            funs.reset()
            oled_display.read_file(main_run_map)
            while True:
                if funs.coutB1 >= 1 :
                    break
            if funs.coutB2 ==1 :
                with open("/map/run_1.txt", "r") as File:
                    last_line = "know"
                    while True:
                        line = File.readline()
                        
                        if not line:  
                            break
                       #print(line.strip())
                        runing(0,line.strip())
                funs.beep(2000,200)
                time.sleep(0.5)
                funs.beep(2000,200)
                time.sleep(0.5)
                funs.beep(2000,200)
            
        if funs.coutB2 % 4 == 2 :
            funs.reset()
            
            while True:
                oled_display.setting_menu()
                
                if funs.coutB1 >= 1 :
                     
                    funs.beep(2000,200)
                     
                    if funs.coutB2 % 4 == 0 :
                        funs.reset()
                        while True:
                            oled_display.cam_bien()
                            ir_sensor.read_all()
                            if funs.coutB1 >= 1 :
                                 
                                funs.beep(2000,200)
                                 
                                if funs.coutB2 % 2 == 0 :
                                    funs.reset()
                                    
                                    choise = "ahead"
                                    while True:
                                        
                                        ir_sensor.read_all()
                                        oled_display.cam_bien_fix(choise)
                                        
                                        if funs.coutB1 % 5 == 4:
                                            funs.beep(2000,200)
                                            funs.reset()
                                            break
                                    
                                if funs.coutB2 % 4 == 1 :
                                    funs.reset()
                                    break
                                
                    if funs.coutB2 % 4 == 1 :
                        funs.reset()
                        time.sleep(0.4)
                        while True:
                            time.sleep(0.01)
                            
                            break
                    if funs.coutB2 % 4 == 2 :
                        funs.reset()
                        while True:
                            oled_display.move_fix()
                            if funs.coutB1 >= 1 :
                                funs.beep(2000,200)
                                #funs.reset()
                                
                                if funs.coutB2 % 4 == 0 :
                                    funs.reset()
                                    while True:
                                        N20_dict_go_ahead = N20.dict_go_ahead + encoder.a_count*5
                                        time.sleep(0.01)
                                        oled_display.goahead_fix(N20_dict_go_ahead)
                                        if funs.coutB1 % 2 == 1:
                                            funs.reset()
                                            break
                                    N20.dict_go_ahead = N20_dict_go_ahead
                                    encoder.reset()
                                    break
                                if funs.coutB2 % 4 == 2 :
                                    funs.reset()
                                    N20_turn_left_long_dis =N20.turn_left_long_dis
                                    N20_turn_left_th2_dis =N20.turn_left_th2_dis
                                    while True:
                                        if funs.coutB2 % 2 == 1:
                                            N20_turn_left_long_dis = N20.turn_left_long_dis + encoder.a_count*5
                                        if funs.coutB2 % 2 == 0:
                                            N20_turn_left_th2_dis = N20.turn_left_th2_dis + encoder.a_count*5
                                        time.sleep(0.01)
                                        oled_display.turn_left(N20_turn_left_th2_dis,N20_turn_left_long_dis)
                                        if funs.coutB1 % 2 == 1:
                                            funs.reset()
                                            break
                                    N20.turn_left_long_dis = N20_turn_left_long_dis
                                    N20.turn_left_th2_dis = N20_turn_left_th2_dis
                                    encoder.reset()
                                   
                                if funs.coutB2 % 4 == 1 :
                                    funs.reset()
                                    N20_turn_right_long_dis =N20.turn_right_long_dis
                                    N20_turn_right_th2_dis =N20.turn_right_th2_dis
                                    while True:
                                        if funs.coutB2 % 2 == 1:
                                            N20_turn_right_long_dis = N20.turn_right_long_dis + encoder.a_count*5
                                        if funs.coutB2 % 2 == 0:
                                            N20_turn_right_th2_dis = N20.turn_right_th2_dis + encoder.a_count*5
                                        time.sleep(0.01)
                                        oled_display.turn_right(N20_turn_right_th2_dis,N20_turn_right_long_dis)
                                        if funs.coutB1 % 2 == 1:
                                            funs.reset()
                                            break
                                    N20.turn_right_long_dis = N20_turn_right_long_dis
                                    N20.turn_right_th2_dis = N20_turn_right_th2_dis
                                    encoder.reset()
                                    
                                
                                if funs.coutB2 % 5 == 3 :
                                    funs.reset()
                                    break
                                
                                funs.reset()
                                
                    if funs.coutB2 % 4 == 3 :
                        funs.reset()
                        break
                            
                
        if funs.coutB2 % 4 == 3 :
            funs.reset()
            while True:
                oled_display.up_file()
                if funs.coutB1 >= 1 :
                     
                    funs.beep(2000,200)
                     
                    
                    if funs.coutB2%6 == 0 :
                        funs.reset()
                        oled_display.oled.fill(0)
                        while True:
                            oled_display.oled.text("clear_all",0,0,1)
                            oled_display.oled.show()
                            if funs.coutB1 >= 1 :
                                 
                                funs.beep(2000,200)
                                 
                                file.delete_all()
                                funs.reset()
                                oled_display.up_file()
                                main_run_map = "filenotf.txt"
                                break
                            
                    if funs.coutB2%6 == 1 :
                        
                        oled_display.oled.fill(0)
                        oled_display.read_file(f"map/run_{funs.coutB2%6}.txt")
                    if funs.coutB2%6 == 2 :
                        
                        oled_display.oled.fill(0)
                        oled_display.read_file(f"map/run_{funs.coutB2%6}.txt")
                    if funs.coutB2%6 == 3 :
                        
                        oled_display.oled.fill(0)
                        oled_display.read_file(f"map/run_{funs.coutB2%6}.txt")
                    if funs.coutB2%6 == 4 :
                        
                        oled_display.oled.fill(0)
                        oled_display.read_file(f"map/run_{funs.coutB2%6}.txt")
                    if funs.coutB2%6 == 5 :
                        
                        oled_display.oled.fill(0)
                        oled_display.read_file(f"map/run_{funs.coutB2%6}.txt")
                    funs.coutB1 = 0
                    while True:
                        if funs.coutB1 >= 1 :
                             
                            funs.beep(2000,200)
                             
                            break
                    main_run_map = f"map/run_{funs.coutB2%5}.txt"
                    break
        funs.reset()
        
#data_init.data.init()
initial_flood()
ir_sensor.distance_init()

# test
#N20.round_back()
#N20.go_one_cell()
#N20.turn_to_left()
#N20.turn_to_right()


#N20.stop()
#machine.reset()
while True:
    ir_sensor.read_all()
    if ir_sensor.adc[3].value>35000:
        
        time.sleep(0.1)
        funs.beep(2000,500)
        time.sleep(0.2)
        
        ir_sensor.adc[0].min_wall = 5000
        ir_sensor.adc[3].min_wall = 5000
        while run_algorithm() != "done":
            oled_display.oled.fill(0)
            oled_display.oled.text(f"{ir_sensor.is_wall_left()} {ir_sensor.is_wall_ahead()} {ir_sensor.is_wall_right()}",0,20)
            oled_display.oled.text(f"{mouse.x} {mouse.y}",0,40)
            oled_display.oled.show()
            funs.beep(2000,50)
        machine.reset()
    
#funs.levelUp()

