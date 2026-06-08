
# Lightweight Robot Simulator and Movement Verifier

A lightweight 3D robot simulation environment designed to assist with robot development, testing, and validation before deploying code to physical hardware.

Originally developed for a hexapod robot project, the simulator can be adapted to virtually any robot platform that relies on inverse kinematics and gait generation. It provides a visual environment for verifying movement algorithms, allowing designers to identify issues early in the development process and reduce debugging time on real hardware.

###*NOTE:
This project is a heavy WIP since it was developed specifically for a hexapod robot I built and has considerable questionable design choices that makes it difficult to model other robots right now. Further development is planned to neaten things up and make it more useable.

[Hexapod_Video.webm](https://github.com/user-attachments/assets/1b0718a9-f99f-4d19-8294-75fef193892c)

## Features

### 3D Robot Simulation
- Real-time visualization using OpenGL and Cinder
- Inspect robot movement from any angle
- Lightweight and easy to modify

### Inverse Kinematics Verification
- Validate IK solutions before deployment
- Identify unreachable positions and joint limitations
- Debug movement algorithms in a visual environment

### Gait Testing
- Test and refine walking patterns and locomotion algorithms
- Verify stability and leg coordination
- Experiment with new movement strategies safely

### Hardware-Ready Code
- Movement and control logic are decoupled from the simulator
- Simulation code can be compiled for a microcontroller with no modification
- Develop once and deploy to both simulation and hardware

### Rapid Prototyping
- Quickly evaluate new robot designs
- Reduce development time and hardware debugging
- Test potentially unsafe movements without risking physical components

## Included Resources

- 3D simulation environment source code
- Example hexapod implementation
- CAD files used for 3D printing the original robot platform

## Technologies Used

- C++
- OpenGL
- Cinder

## Example Applications

- Hexapod robots
- Quadruped robots (further development is planned for use on other robots)
- Custom robotic platforms
- Inverse kinematics research
- Gait development and testing
- Educational robotics projects
- Motion planning experimentation
