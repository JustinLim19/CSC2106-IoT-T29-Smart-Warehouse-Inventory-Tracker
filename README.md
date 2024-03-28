# Smart Warehouse Inventory Tracker

The Smart Warehouse Inventory Tracker is a project developed by Liew Jun Wei, Abdul Halim Bin Abdul Rahim, Wong Kar Long Keagan, Mirza Anaqi Bin Muhammad Haizan, and Ang Guo Zheng Frederick as part of the Computing Science Joint Degree Programme at the Singapore Institute of Technology - University of Glasgow. The project focuses on developing a comprehensive Smart Warehouse Inventory Tracker for indoor environments using Bluetooth Low Energy (BLE) and microcontroller units (MCUs) such as M5StickC Plus and Raspberry Pi.

## Introduction
The Smart Warehouse Inventory Tracker project aims to provide precise location information of assets within indoor environments in real time. By leveraging BLE and MCUs, the system facilitates accurate three-dimensional tracking of assets, addressing challenges associated with vertical tracking.

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
