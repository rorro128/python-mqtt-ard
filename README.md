# python-mqtt-ard

MQTT Queue Service fo IoT. Using Arduino, Raspberry Pi 4, Python, and humidity and distance sensors. The Arduino device acts as an MQTT publisher with the sensors sending information every 10 seconds to the Broker configured as a service on the Raspberry Pi 4. The devices Laptop1 and Mobile1 act as MQTT subscribers to the topic "testTopic," receiving the information in the Python process and the MQTT IoT panel for Android.

For future improvements:
- Security and authentication can be added for topics. 
- Create new topics to separate the use of sensors. 
- Enable an LCD on Arduino to display information of topics.
- Activate Android panel modules to make the Android device a Publisher.