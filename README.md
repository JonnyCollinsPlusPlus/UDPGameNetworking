# **UDPGameNetworking Library**
A lightweight C++ UDP networking solution designed specifically for game development, featuring reliable messaging and interpolation without engine dependencies.

### **Features**
- **Generic data streaming over UDP** (Define custom data types, with custom serialization and deserialization methods, and let the library do the rest)
- **Networked value lerping** (Easily toggle in Library settings, customizable delay, smooth networked objects even with rocky connections)
- **Important message confirmation** (Custom user messages can use message acknowledgement, even over UDP)
- **Engine agnostic design** (Designed to be wrapped for use with any game engine, see Status)
- **Simple demo to showcase engine functionality** (Fully functional example showing core capabilities)

### **Demo**
- Two clients running simultaneously on one system
- Each client controls a square with real time position streaming
- **Controls**:
	- 'WASD' Move your square
	- 'Mouse Hover' Select which client window to control
	- 'M' send a message from the selected client, see result in settings window
- Change settings and see how the behaviour changes

### **Installation**
- clone the repo, build with cmake, and run

### **Status**
- **Working**:
- Demo fully functional
- Core networking is stable
- **Limitations**:
- Localhost tested only (not yet LAN)
- No NAT traversal implementation or planned

### **Planned Improvements**
- Cleanup and optimization
- Wrapper implementations for Godot and Unity
- Additional documentation
