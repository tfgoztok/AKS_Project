# AKS (Atış Kontrol Sistemi / Fire Control System) Project

## Table of Contents
1. [Introduction](#introduction)
2. [Project Structure](#project-structure)
3. [Features](#features)
4. [Installation](#installation)
5. [Usage](#usage)
6. [Configuration](#configuration)
7. [Communication Protocol](#communication-protocol)


## Introduction

The AKS (Atış Kontrol Sistemi / Fire Control System) is a sophisticated software system designed to control and manage the firing process of missiles on a shooting platform. This project implements a simplified version of an AKS, demonstrating key functionalities such as missile management, communication with subsystems, and a graphical user interface for monitoring and control.

## Project Structure

```
AKS_Project/
├── AKS_Project.pro
├── README.md
├── src/
│   ├── AKSApp.cpp
│   ├── AKSApp.h
│   ├── AKSGUI.cpp
│   ├── AKSGUI.h
│   ├── ClickableMissileWidget.cpp
│   ├── ClickableMissileWidget.h
│   ├── CommunicationManager.cpp
│   ├── CommunicationManager.h
│   ├── ConfigReader.cpp
│   ├── ConfigReader.h
│   ├── GlobalConstants.h
│   ├── Logger.cpp
│   ├── Logger.h
│   ├── main.cpp
│   └── AKSCore.cpp
│   └── AKSCore.h
├── ui/
│   └── mainwindow.ui
├── simulators/
│   └── ANSSimulator.py
│   └── LauncherSimulator.py
├── config/
│   └── missiles.conf
```

## Features

1. Real-time communication with Inertial Navigation System (ANS) and Launcher subsystems
2. Graphical User Interface (GUI) for system monitoring and control
3. Missile status management (health, power, firing)
4. Platform and target information display
5. Configurable missile setup through configuration file
6. Logging system for tracking events and errors
7. Simulated ANS and Launcher subsystem for testing and demonstration purposes

## Requirements

- Qt 5.x or later
- C++11 or later
- Python 3.x (for ANS and Launcher simulator)
- Windows OS

## Usage

1. Start the AKS application:
   ```
   Run the AKS_Project.exe under ./build folder. All necesarry library files are created with windeployqt.exe
   Note that config/missiles.conf should be inside executable file's root folder.
   ```

2. Run the ANS simulator:
   ```
   python simulators/ANSSimulator.py and python simulators/LauncherSimulator.py
   ```

3. Use the GUI to interact with the system, monitor missile statuses, and perform launch operations.

4. Read ./build/aks_log for any detailed information.

## Configuration
The missile configurations are loaded from a file named `missiles.conf`. The format of the configuration file is as follows:
```
# Format: MissileIndex,MissileName,IsHealthy
0,,
1,J-600T,true
2,SOM A,true
3,Bora,true
```

## Communication Protocol

This project uses UDP (User Datagram Protocol) for communication between the AKS and its subsystems (ANS and Launcher). UDP was chosen over TCP for the following reasons:

1. Low latency: UDP offers lower latency compared to TCP, which is crucial for real-time systems like a fire control system.
2. Simpler implementation: UDP's connectionless nature simplifies the implementation, reducing complexity in the communication layer.
3. Periodic updates: The system primarily deals with periodic updates (e.g., heartbeats, position information) where the occasional loss of a packet is less critical than consistent, timely delivery.
4. Multicast support: While not utilized in this implementation, UDP's support for multicast could be beneficial for future expansions of the system.