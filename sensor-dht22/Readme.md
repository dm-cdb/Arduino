This is a minimal Arduino code for the DHT22 ; it uses around 4,5KB of flash memory.

Please note that a large number of DHT22 sold are clones/licensed products from the original manufacturer ASAIR ; so the specifications are NOT guaranteed with them<br>
Below is what a guenine ASAIR DHT22/AM2302 looks like :<br>

![ASAIR-AOSONG-DHT22](https://github.com/user-attachments/assets/f2a1c9e9-13f9-4c31-b12f-b805f9de34f1)


The circuit is similar to the DHT11 : a 5K-10K Ohms resistor maintains the data communication line HIGH like this :

![DHT22-schematic](https://github.com/user-attachments/assets/3567fab1-04d3-49ec-b5af-77de532e0e85)

Again many sensors are sold on a breakout boards with an embeded pull up resistor ( R1 - around 5K Ohms) - so they are just ready to go without additional components.<br>
Below is an example with an embeded 5.1K Ohms resistor (+ a small by pass capacitor C1 to secure a stable VCC).

![DHT22-breakout](https://github.com/user-attachments/assets/0b220d2d-3034-4a03-b591-8b74946bc09e)


This DHT22 code has been tested in a freezer ; we don't have the same bug as the DHT11 with negative temperature, where the latter could not make a difference between -3,0°C and -4,0°...<br>

![dht22-freezer](https://github.com/user-attachments/assets/132fcd4c-4345-4209-ac41-e45edd3bd174)
