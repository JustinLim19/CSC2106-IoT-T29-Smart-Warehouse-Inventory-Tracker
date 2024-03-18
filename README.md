# Smart Warehouse Inventory Tracker

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
1. In the terminal run "npm install"
2. CD to UI folder and type "node app.js"
3. In web browser open link to "http://localhost:3000/"