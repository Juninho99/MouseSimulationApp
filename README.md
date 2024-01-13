# MouseSimulationApp
 
## Project Overview
This project is divided into two components. The first part involves the creation of a mobile application, while the second part is centered around the Mbed simulator. The mobile application is designed to send specific data, from its sensors, to an MQTT broker by publishing it to a predefined topic. The received data is then visualized through the Mbed simulator.

### Mobile Application
- Developed in Java using Android Studio.
- Publishes data to an MQTT broker on a specific topic.

### Mbed Simulator
- Utilizes C code for mouse movement visualization.
- Subscribes to the same specific MQTT topic on the broker.
- Retrieves and visualizes data sent from the mobile application.

Interested individuals can view YouTube video demonstrations of the application

https://www.youtube.com/watch?v=WK2tN-f0FGA&t
