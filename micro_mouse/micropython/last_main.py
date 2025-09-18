import pin_bot, N20, encoder ,funs , time, machine,oled_display, PID ,ir_sensor,file,PID                                                          

# set up
#funs.levelUp()

main_run_map = "filenotf.txt"

while True :
    
    oled_display.main_menu()
    
    if funs.coutB1 >= 1 :
         
        funs.beep(5000,200)
         
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
                funs.beep(5000,200)
                break
            
        if funs.coutB2 % 4 == 2 :
            funs.reset()
            
            while True:
                oled_display.setting_menu()
                
                if funs.coutB1 >= 1 :
                     
                    funs.beep(5000,200)
                     
                    if funs.coutB2 % 4 == 0 :
                        funs.reset()
                        while True:
                            oled_display.cam_bien()
                            ir_sensor.read_all()
                            if funs.coutB1 >= 1 :
                                 
                                funs.beep(5000,200)
                                 
                                if funs.coutB2 % 2 == 0 :
                                    funs.reset()
                                    while True:
                                        oled_display.remove("value")
                                        funs.value_temp = encoder.a_count
                                        time.sleep(0.01)
                                        if funs.coutB1 >= 1 :
                                            funs.beep(5000,200)
                                            funs.reset()
                                            break
                                    
                                if funs.coutB2 % 4 == 1 :
                                    funs.reset()
                                    break
                                
                    if funs.coutB2 % 4 == 1 :
                        funs.reset()
                        time.sleep(0.4)
                        while True:
                            oled_display.toc_do()
                            time.sleep(0.01)
                            if funs.coutB2%2 == 0 :
                                N20.a=1+encoder.a_count*0.0001
                            if funs.coutB2%2 == 1 :
                                N20.b=1+encoder.a_count*0.0001
                            if funs.coutB1 == 1 :
                                funs.reset()
                                funs.beep(5000,200)
                                break
                    if funs.coutB2 % 4 == 2 :
                        funs.reset()
                        while True:
                            oled_display.encoder_mode()
                            if funs.coutB1 >= 1 :
                                funs.beep(5000,200)
                                break
                    if funs.coutB2 % 4 == 3 :
                        funs.reset()
                        break
                            
                
        if funs.coutB2 % 4 == 3 :
            funs.reset()
            while True:
                oled_display.up_file()
                if funs.coutB1 >= 1 :
                     
                    funs.beep(5000,200)
                     
                    
                    if funs.coutB2%6 == 0 :
                        funs.reset()
                        oled_display.oled.fill(0)
                        while True:
                            oled_display.oled.text("clear_all",0,0,1)
                            oled_display.oled.show()
                            if funs.coutB1 >= 1 :
                                 
                                funs.beep(5000,200)
                                 
                                file.delete_all()
                                funs.reset()
                                oled_display.up_file()
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
                             
                            funs.beep(5000,200)
                             
                            break
                    main_run_map = f"map/run_{funs.coutB2%5}.txt"
                    break
        funs.reset()
N20.begin()
N20.goahead(500)
funs.wait()
