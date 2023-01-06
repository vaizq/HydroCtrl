![HydroCtrl-logo](https://user-images.githubusercontent.com/68817622/210755586-5aa0ddc7-f66c-4db1-8e68-a135ee4e9bea.png)

## Pitch
### Problem
<p>Let $G$ be the genetic potential of a strain.
$E_n(t)$ be a function that describes error of a cultivation variable $n$ at time $t$ compared to absolute optimal.
Finally let $R$ be the result of the cultivation cycle.
Then I'm going to state that $R = G - \sum (\int E_n(t) dt)$.</p>
<p>Goal of HydroCtrl is to minimize $|R-G|$.</p>

### Solution
<strong>Extensible iot-nodes based architecture.</strong>
Every greenhouse is different and have different needs for monitoring and automation.
HydroCtrl provides a platform where users can select nodes they need and just plug them into the system.
HydroCtrl is just an API, server and UI. We do provide nodes, but encourage users to create new nodes
for different purposes and budgets.

## Design
HydroCtrl is composed of three main compoments:
- Client 
- Server 
- Iot-nodes

## Capabilities
In practise IOT nodes can do whatever, but from UI they can be configured to do following things.
### Nodes
- Read unlimited number of ph, ec and oxygen sensors.
- Control unlimited number of pumps, fans and solenoids.
### Server

### UI
- Dashboard
- Rule builder/automation editor



