# HydroCtrl

## Brief
HydroCtrl is a software infrastructure that enables people to build customized hydroponics automation and monitoring systems.
HydroCtrl achieves this by using master and slave nodes based architecture.
HydroCtrl provides API so users than create their own nodes and integrate them into their systems.
HydroCtrl provides a powerfull UI so users can monitor and configure their systems.

# Philosophy 
HydroCtrl should be simple for simple tasks and powerfull enough for the most advanced idustrial use. (STL mentality)
HydroCtrl should be extensible and customizeable.
HydroCtrl is not a data visualization app.

## Detailed constraints
API for nodes
Greenhouse monitoring
Automation editor
Notifications and alerts
Testing environment for nodes.

## Sensors HydroCtrl can handle
- Every sensor that's output can be represented as a vector of real numbers

## Devices HydroCtrl can control
- Lights
- Pumps
- Fans
- Valves

## Software design
Use simple object oriented modern c++.
We use c++ for a number of reasons:
- I'm most familiar with it
- HydroCtrl has realtime contraints 
- Ability to create customizeable GUI that can run in desktop and web. (emscripten, sdl2, imgui)
- C++ is used in large scale real word projects

We use object oriented paradigm so we can use object oriented patterns and make HydroCtrl more extensible and modular.
If there is a library for a task consider using it.
Write tests whenever possible.
Use git for version control.
If it does not feel right it's not propably right.
Don't be afraid to change architecture.
Don't optimize without reason.
Error handling is important.

## Things I need to learn before I start implementing
1. API design
2. Testing
3. Version control

## Action steps
- Start studing about professional software design (concurrent)
- Create a prototype/MVP (Minimum viable product) 
- Start testing the prototype in a real environment (concurrent)
- Design and document final project
- Start campaigns and adverticing
- Start buiding the final project
