# RC receiver nRF24L01 
RC receiver nRF24L01 with ATmega328P processor for smaller dimensions. It is possible to use Arduino Nano, Pro or Pro Mini.
It also includes a MX1508 motor driver with brake and a CMT2150A transponder for laps timing.
Other versions of the RX firmware include servo outputs with 8 and 16 bit timers.
The firmware will be used for racing micro cars and boats.
After editing the code, it can control tanks and aircraft.
The possibility of setting the brake is in the code.
Telemetry sends the monitored voltage RX to TX using LED states. 

This RC receiver works with a simple RC transmitter from my repository https://github.com/stanekTM/RC_TX_nRF24L01_Lite_Telemetry_LED

## RC receiver with motor drivers (no servos)
1. Motor A (pwm 1, 2) = 3.9 kHz (steering) 
2. Motor B (pwm 3, 4) = 3.9 kHz (throttle) 
3. Normal mode = LED RX is lit
4. Battery voltage 1S LiPo (4.2V) < 3.3V = RX, TX LEDs flash at a frequency of 500ms
5. TX transmitter off or signal loss = RX LED flash at a frequency of 100ms 
6. Fail-safe = Motor A and B stopped

![screen1](https://github.com/stanekTM/RC_RX_nRF24L01_Lite_Telemetry_Motor_Driver_Servo/blob/master/RC_RX_nRF24L01_Lite_Telemetry_Motor_Driver/RC_RX_nRF24L01_Lite_Telemetry_Motor_Driver.PNG)

George StanekTM
