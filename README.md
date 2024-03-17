# Smart Warehouse Inventory Tracker

## M5StickC Plus BLE Node Setup

This repository contains code for setting up Bluetooth Low Energy (BLE) nodes using M5StickC Plus devices. These nodes include an AssetNode and CornerNode functionality.

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
