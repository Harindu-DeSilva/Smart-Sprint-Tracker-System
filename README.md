# Smart-Sprint-Tracker-System

The Smart Sprint Tracker System represents an Arduino-based project which developed through the Higher National Diploma in Software Engineering under its Robotics Application Development module. The system helps athletes through automatic sprint lap time and speed measurement to eliminate human errors that occur during manual timing procedures. The system utilizes Arduino Uno together with IR sensors and LCD with I2C module and Bluetooth module (HC-05) and keypad to deliver a completely automated sprint tracking solution at an affordable cost.

The system enables remote control through Bluetooth which allows athletes to use their smart devices for operation. The system allows users to establish lap counts and begin sessions and view live lap information without requiring help from other people. The device functions in indoor sprinting areas with IR sensors as its main operating environment to improve athletic training effectiveness through precise customizable sprint performance monitoring.

*Methodology*
The project utilized a systematic approach through multiple essential phases for its development.

*System Architecture Design*

Defined all required hardware and software components.

The team established the path that data follows between system inputs from users and sensor signals and system outputs.

*Component Integration*

Centralized around the Arduino Uno for control and data processing.

The system employed IR sensors for start and finish line detection of sprinter movements.

Users could enter lap count and distance parameters through the 4x4 Keypad system.

The system integrates an LCD Display through I2C communication to reduce wiring needs and show real-time output.

The implementation of HC-05 Bluetooth module enabled remote system communication.

The system includes LEDs and a Buzzer which produces audible and visual notifications for lap completion and special modes.


*Main Functional Phases*

Initialization Phase: User inputs the sprint session parameters through the keypad.

The system tracks laps through IR sensors to determine lap time during this phase.

During the display phase the LCD screen presents sprint information including lap time and average speed and total laps completed.


