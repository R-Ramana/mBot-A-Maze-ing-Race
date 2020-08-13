# mBot-A-Maze-ing-Race
A robot that was calibrated to traverse a maze by responding to colour and frequency cues. Created using mBot (Arduino IDE).
You can read the file A-Maze-ing_Race_Report.pdf to get comprehensive details and information about our project and challenges faced. Images and diagrams are attached in the pdf for better visualization.

# Feature List
  - Colour Detection
    - RGB LED Sensors
    - Light Dependent resistors

  - Frequency Detection
    - Microphone

  - Collision Detection
    - Ultrasonic Sensors
    - Infrared Sensors

# Overall Algorithm
When the button on the mBot was pressed, it immediately begins moving forward. Upon detecting a black strip below, it stops. The LED is activated, which then scans for a colour above the mBot. Depending on the colour it detects, apart from black, the mBot’s motor will cause it to move in a certain direction. If the LED scans black, the microphone on the mBot will be activated. The mBot will then scan for an audio. If a high or low frequency is played, the mBot will again move accordingly. If no audio is played, i.e. the mBot has reached the end of the maze, a victory sound will be played. Infrared and Ultrasonic sensors were used to prevent the mBot from colliding with the boundaries and keep it on a straight path. You can find the flowchart in the  A-Maze-ing_Race_Report.pdf file.
