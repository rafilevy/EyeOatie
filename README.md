# EyeOatie

## Introduction

EyeOatie is a platform to easily create and deploy custom IoT devices. It uses WiFi mesh networks to create a strong connection between devices and to isolate devices from the main network. There is also one bridge device which is connected to the mesh along with the main network from where the devices can be monitored and controlled.

The mesh network is setup using the [painlessMesh](https://gitlab.com/painlessMesh/painlessMesh) library for ESP devices.

### Node

EyeOatie nodes are the IoT devices themselves. Nodes are comprised of 3 parts:

* Actions:

   Outputs the device can perform (e.g. turning a light on)

* Data:

   Any piece of data the device is able to measure such as temperature through the use of a thermometer module.

* Events:

   The device signalling that an event has taken place such as a piece of data going above a certain value.

All these parts can be controlled using the public methods of the EyeOatieNode class, see below for more details.

### Bridge

The bridge device connects the node mesh network to the main network. The bridge devices role is simply to relay messages from the mesh to the server and vice versa. This can most easily be done through a Serial connection to the server device.

### Server

The server allows a user to communicate with the devices. It provides an interface to perform device actions, see device data and also to link events to actions with other conditions. The server communicates with the mesh via serial with the bridge device.

___

## Docs

### EyeOatieNode

EyeOatieNode is the main node class.

```c++
EyeOatieNode::EyeOatieNode(String name, String meshSsid, String meshPassword)
```

The constructor for node devices. Returns an EyeOatieNode object.

* **String name**: The name of the device.
* **String meshSsid**: The ssid of the mesh network to be used by devices. Defaults to `"EyeOatieMesh"` if this parameter is not included.
* **String meshPassword**: The password to the mesh network. Has a default value but it is reccomended this is changed so the network cannot be externally accessed.

```c++
void EyeOatieNode::addData(String name, String type, std::function<String()> getData)
```

Registers a piece of data on the node.

* **String name**: The name of the data. This must be unique on the device.
* **String type**: The type of the data, used for conversion by the server.
* **std::function<String()> getData**: a function which returns the piece of data converted to a String. e.g. a function which reads data from a sensor attached to the device.

```c++
void EyeOatieNode::addAction(String name, std::function<void()> actionCallback)
```

Registers an action on the node.

* **String name**: The name of the action. This must be unique on the device.
* **std::function<void()> getData**: The function to be run when the action is executed. e.g. a function which digitalWrites to a pin on the device.

```c++
void EyeOatieNode::update()
```

Passthrough of the painlessMesh update function. This is to be placed in the loop function of the device's main sketch.

### EyeOatieSerialBridge

The class for a bridge device connected to the server via a serial connection.

```c++
EyeOatieSerialBridge::EyeOatieSerialBridge(String meshSsid, String meshPassword)
```

The constructor for bridge devices. Returns an EyeOatieSerialBridge object.

* **String meshSsid**: The ssid of the mesh network to be used by devices. Defaults to `"EyeOatieMesh"` if this parameter is not included.
* **String meshPassword**: The password to the mesh network. Has a default value but it is reccomended this is changed so the network cannot be externally accessed.

```c++
void EyeOatieNode::update()
```

Updates the painlessMesh and handles any incoming serial data. This is to be placed in the loop function of the device's main sketch.
