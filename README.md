# Smart Warehouse Inventory Tracker

The Smart Warehouse Inventory Tracker is a project developed by Liew Jun Wei, Abdul Halim Bin Abdul Rahim, Wong Kar Long Keagan, Mirza Anaqi Bin Muhammad Haizan, and Ang Guo Zheng Frederick as part of the Computing Science Joint Degree Programme at the Singapore Institute of Technology - University of Glasgow. The project focuses on developing a comprehensive Smart Warehouse Inventory Tracker for indoor environments using Bluetooth Low Energy (BLE) and microcontroller units (MCUs) such as M5StickC Plus and Web Interface. 

## Introduction
In the ever evolving landscape of logistics, the precise tracking of warehouse assets not only affords convenience, but is also strategically essential in the efficient warehouse operation. While various technologies like Ultra-Wideband (UWB), RFID, and Wi-Fi have made advancements in indoor positioning, accurately tracking an object’s vertical location in indoor environments remains a challenge.

## Problem Statement
The primary challenge this project aims to address is the lack of accurate and reliable indoor asset tracking, particularly in the vertical dimension. Current indoor positioning technologies, such as RFID, Wi-Fi, and Bluetooth beacons, struggle to provide precise 3D location information, hindering effective warehouse management and asset visibility. Accurately tracking the vertical position of assets within a complex, multi-floor indoor environment remains a significant limitation that needs to be addressed.

## Objectives
The Smart Tracker project tackles the challenge of precise 3D asset tracking within warehouses. It accomplishes this by setting up a dense network of BLE nodes for comprehensive coverage and utilizes a trilateration algorithm to pinpoint an asset's location based on signals from these nodes. This provides not only horizontal but also vertical location data. Real-time data processing ensures constant updates, and the project prioritizes evaluating the system's accuracy, reliability, and scalability for a robust solution.

## System Architecture
Our proposed solution aims to utilise BLE connectivity to measure Received Signal Strength Indicator (RSSI) values to calculate and estimate the position of the tracked item within an indoor space. The system architecture consists of three main components: the Asset Nodes, Corner Nodes and the Web Interface.
/Users/halimsmacbookairm1/Downloads/iot_sysarch.png/Users/halimsmacbookairm1/Downloads/iot_layout.png
The system architecture, as depicted in Figure 1, illustrates the general layout of the proposed solution. Each Corner Node will be placed in each corner of a room. An additional Corner Node will be placed at the rooms tallest point. An object that is attached to an Asset Node will calculate its relative position based on trilateration. After its position has been determined, the Asset Node will send its locational data to an interface. Each Asset Node will send its locational data to the interface. This interface, as visualised in Figure 2, is designed to display the positions of all Asset Nodes in the rooms vicinity. 1) Asset Nodes: The item to be tracked is equipped with an Asset Node, which is a small, BLE-enabled device. The Asset Node uses BLE to actively scan for nearby Corner Nodes. Once connected to the Corner Nodes, it will periodically retrieve and record the RSSI values from the Corner Nodes within the indoor space. By retrieving these signals, the RSSI value can be measured for each respective ping from a Corner Node. The Asset Node will then be able to derive its own position relative to the fixed Corner Nodes. After deriving its own position, the Asset Node sends its own positional data to a web server through HTTP. 2) Corner Nodes: The system employs a network of strategically placed Corner Nodes throughout the indoor environment. The Corner Nodes broadcasts bluetooth signals periodically, transmitting these signals to any Asset Nodes that are in the vicinity. The Corner Nodes act as anchor points for the trilateration algorithm, providing the necessary reference points to determine the 3D position of the Asset Nodes. 3) Server: A server application handles any incoming JSON data sent over by the Asset Node over HTTP. The JSON data contains information such as Node ID, and the positional data of the Asset Node. This data is then stored in an SQLitedatabase, so that positional data is readily available and canbe easily fetched by the web interface. 4) Web Interface: The web interface displays the location of each Asset Node within the defined space of the Corner Nodes. It provides a visual representation of the position ofeach Asset Node in the indoor environment. The interface should also enable user interaction by allowing the rotation of the rooms axes. Locational data is updated in real-time, which shows the adjusted position of the Asset Node, if there is any change. 5) Architecture: The system architecture illustrates the general layout of the proposed solution. Each Corner Node will be placed in each corner of a room. An additional CornerNode will be placed at the rooms tallest point. The item to be tracked is attached to an Asset Node, which will calculate its relative position based on trilateration. After its position has been determined, the Asset Node will send its locational data to an interface via HTTP. Each Asset Node will send its locational data to the interface. The interface will then display the position of all Asset Nodes in the rooms vicinity.

## M5StickC Plus BLE Node Setup

### How to Set Up

#### AssetNode Setup
1. Upload `AssetNode.ino` to an M5StickC Plus device.

#### CornerNode Setup
1. Open `CornerNode.ino` in your preferred Arduino IDE.
2. For each CornerNode device, follow these steps:
   - In the code, locate the section where the device name is defined.
   - Uncomment and set the `bleServerName` according to the corner node number. For example:
     ```c
     #define bleServerName "CornerNode1"
     // #define bleServerName "CornerNode2"
     // #define bleServerName "CornerNode3"
     // #define bleServerName "CornerNode4"
     ```
3. Upload the modified `CornerNode.ino` to the respective M5StickC Plus device.

#### UI Setup
Basic Node Setup: https://nodejs.org/en/learn/getting-started/introduction-to-nodejs
1. In the terminal run "npm install"
2. CD to UI folder and type "node app.js"
3. In web browser open link to "http://localhost:3000/"

## Acknowledgment
We would like to express our gratitude to our project team members for their contributions: Liew Jun Wei, Abdul Halim Bin Abdul Rahim, Wong Kar Long Keagan, Mirza Anaqi Bin Muhammad Haizan, and Ang Guo Zheng Frederick.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

## Contact Information
For inquiries or support related to the Smart Warehouse Inventory Tracker project, please contact:
- Liew Jun Wei (2837290L@student.gla.ac.uk)
- Abdul Halim Bin Abdul Rahim (2837343A@student.gla.ac.uk)
- Wong Kar Long Keagan (2837260W@student.gla.ac.uk)
- Mirza Anaqi Bin Muhammad Haizan (2837305m@student.gla.ac.uk)
- Ang Guo Zheng Frederick (2837391A@student.gla.ac.uk)
