    import machine

    # Định nghĩa các chân GPIO

    #Chân đọc ss
    device_1_xshut = machine.Pin(9, machine.Pin.OUT)

    device_1_xshut.value(0)

    adc_pins = [machine.ADC(26), machine.ADC(27), machine.ADC(28), machine.ADC(29)] #[A0,A1,A2,A3]

    #chân điều khiển động cơ

    PwmB = machine.PWM(machine.Pin(0))
    Bin2 = machine.Pin(2, machine.Pin.OUT)
    Bin1 = machine.Pin(1, machine.Pin.OUT)
    Stby = machine.Pin(3, machine.Pin.OUT)
    Ain1 = machine.Pin(4, machine.Pin.OUT)
    Ain2 = machine.Pin(5, machine.Pin.OUT)

    PwmA = machine.PWM(machine.Pin(6))
    PwmB.freq(1000)
    PwmA.freq(1000)
    #encoder


    ea1 = machine.Pin(18, machine.Pin.IN, machine.Pin.PULL_DOWN)
    ea2 = machine.Pin(19, machine.Pin.IN, machine.Pin.PULL_DOWN)

    eb1 = machine.Pin(22, machine.Pin.IN, machine.Pin.PULL_DOWN)
    eb2 = machine.Pin(23, machine.Pin.IN, machine.Pin.PULL_DOWN)

    #chân chức năng

    ir_pin = machine.Pin(16, machine.Pin.OUT)

    Buzzer = machine.PWM(machine.Pin(7))
    LED_PIN = machine.Pin(8, machine.Pin.OUT)
    Buzzer.freq(400000)

    BUTTON_1 = machine.Pin(24, machine.Pin.IN, machine.Pin.PULL_DOWN)
    BUTTON_2 = machine.Pin(25, machine.Pin.IN, machine.Pin.PULL_DOWN)

    i2c = machine.I2C(0, scl=machine.Pin(21), sda=machine.Pin(20))


    # Hàm khởi tạo

    def initialize():
        LED_PIN.off()
        Stby.on()
