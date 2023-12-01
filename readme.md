# Robot Butterfly

![robot butterfly handmade project with 3D printed wings and a sensor on the front of the enclosure](http://robotzwrrl.xyz/images/splash/splash007.jpg)

Project using RP2040 during the [Making Embedded Systems class](https://classpert.com/classpertx/courses/making-embedded-systems/cohort) Orange Stars cohort. :)

## Individual firmware tests:

| Test                                         | Result | Code                         |
| -------------------------------------------- | ------ | ---------------------------- |
| Blink LED                                    | PASS   | blinktest                    |
| Neopixels on, off, set individual colours    | PASS   | neotest                      |
| Button interrupt fired on press & release    | PASS   | buttontest                   |
| Button interrupt rudimentary debounce        | PASS   | buttontest                   |
| Servo moves full range                       | PASS   | servotest                    |
| Two servos move simultaneously               | PASS   | servotest                    |
| ADC reading ultrasonic sensor                | PASS   | adctest                      |
| Timer interrupt one time fire                | PASS   | timertest                    |
| Timer interrupt repeat fire                  | PASS   | timertest                    |
| Sleep mode activate & deactivate             | PASS   | sleeptest                    |
| Dual core test                               | PASS   | dualcoretest                 |
| Button debounce                              | PASS   | debouncetest                 |
| Button cycle neopixel colours                | PASS   | neocycletest                 |
| Servos movement using interrupt              | PASS   | servospeedtest               |
| Servos change speed                          | PASS   | servospeedtest               |
| Servos moving opposite positions             | PASS   | servospeedtest               |
| Ultrasonic sensor distance value             | PASS   | ultrasonicsensortest         |
| Ultrasonic sensor sample on interrupt        | PASS   | ultrasonicsensortest         |
| Ultrasonic sensor on 2nd core                | PASS   | ultrasonicsensordualtest     |
| Ultrasonic sensor improve latency w/2nd core | PASS   | ultrasonicsensorduafastltest |

