# Kernel Whisperer

## What is it?

Kernel Whisperer encompasses four components:
* Kernel module: captures events and creates internal strings with enough information to identify the sources of those events. The events are stored internally using a queue implemented using a linked list.
* User-mode application: user-mode application that fetches event strings from the aforementioned queue and stores them on a database.
* Detours dll loaded on all processes through APPInit registry key
* Database (SQL, No-SQL, NewSQL, Graph): database where the kernel events are stored. 

I have developed this tool as a means to collect kernel events in a way that could allow for mining, analytics, etc. 

## Which events are being caught?
* Filesystem:
	* Created Files
* Network:
	* TCP: outbound/inbound connect/data 
	* UDP outbound/inbound data
	* ICMP outbound/inbound data
* Process creation/termination
* Registry:
	* Create Key (RegCreateKey, RegCreateKeyEx)
	* Set Value (RegSetValue, RegSetValueEx)
	* Query Key metadata (RegQueryInfoKey) 
	* Query Value (RegQueryValue, RegQueryValueEx)
	* Open Key (RegOpenKey, RegOpenKeyEx)
* Loaded images (i.e. executables, dlls)
* Operations with object handles (e.g. creation/duplication of process/thread handles)
* API calls (only a few but extendable)
	
More events will be added to Kernel Whisperer as needed.

## How are the tables organized?
* The database is called Events.
* There are seven tables:
	* File: ID, Timestamp, Hostname, PPid, PImageFilePath, Pid, ImageFilePath, Type (FILE_CREATED, FILE_OPENED, FILE_OVERWRITTEN, FILE_SUPERSEDED, FILE_EXISTS, FILE_DOES_NOT_EXIST), File
	* Registry: ID, Timestamp, Hostname, PPid, PImageFilePath, Pid, ImageFilePath, Type (CREATEKEY, OPENKEY, QUERYKEY, QUERYVALUE, SETVALUE, OPENKEY), RegKey, Value, Data
	* Network: ID, Timestamp, Hostname, PPid, PImageFilePath, Pid, ImageFilePath, Protocol (TCP, UDP, ICMP, UNKNOWN), Type (CONNECT, RECV/ACCEPT, LISTEN), LocalIP, LocalPort, RemoteIP, RemotePort
	* Process: ID, Timestamp, Hostname, PPid, PImageFilePath, Pstatus (Started, Terminated), Pid, ImageFilePath, CommandLine
	* ImageLoads: ID, Timestamp, Hostname, PPid, PImageFilePath, Pid, ImageFilePath, HostProcessPid, HostProcessImageFilePath, LoadedImage (i.e. path for loaded file)
	* Objects: ID, Timestamp, Hostname, PPid, PImageFilePath, Pid, ImageFilePath, ObjPid, ObjImageFilePath, ObjType (currently PROCESS or THREAD), HandleOperation (creation or duplication), Permissions (human-readable representation of requested permissions)
	* API: ID, Timestamp, Hostname, PPid, PImageFilePath, Pid, ImageFilePath, Function,

ID is an auto-incremented integer generated for each entry upon insertion. I am using the tuple ID+Timestamp to identify a record since KeQuerySystemTime does not provide a timestamp with enough precision to avoid collisions (i.e. events on the same table with the same timestamp).

**Note:** Currently, Kernel Whisperer will drop any database called Events before starting the insertions. You can tune this behavior by adjusting the queries on Client->lib->SQLDriver (header and source file).

## Interaction with the database
I am not leveraging any API to interact with the database. I wanted Kernel Whisperer to be versatile so that i could switch to any database with a couple of adjustments. As such, i have created a simple Python server that reads queries and executes them directly on the OS through the command line. 


## Ports, configurations and modules
* By default, the Python server runs on port 5000 and the client will interact with that same port. The port and IP address for the database can be changed on Client->lib->SQLDriver.

## About Detours
Detours is a Microsoft library that you can leverage to intercept calls to OS functions (e.g. CreateFile). This interception is achieved through a dll created by the developper and then loaded within applications. From then on, any calls to detoured functions will be first handled by the dll. As expected, Kernel Whisperer does not intercept every single function provided by Windows but it can be easily extended with functionality as needed. I am currently intercepting:
* WriteProcessMemory
* AdjustTokenPrivileges

## How to compile and run Kernel Whisperer?
Start by creating a database on MySQL and running the Python proxy i provide on this project. Make sure you take note of the IP address for the host running the database. You need to change it on Kernel Whisperer's source (Client->lib->SQLDriver).

1. Disable Windows driver integrity check and/or enable test signatures. This depends on the operating system. Starting on Windows 7 x64 you need to sign the driver using a Certificate generated by you. For older versions, it is enough to disable integrity verification. Details of the process will be omitted (Google is your friend here).
2. Install:
  * WDK 7.1.0 (https://developer.microsoft.com/en-us/windows/hardware/windows-driver-kit)
  * Visual Studio or the developper tools
3. Compiling and deploying driver:
   1. Start->Windows Driver Kits->WDK [VERSION]->Build Environments->Windows 7->x64 Checked Build Environment
   2. Change directory to the folder where Kernel Whisperer is located
   3. Run CompileAndDeployDriver.cmd. **NOTE:** This script will generate the certificate (amongst other stuff) and save it on Driver->staging. Once you generate the certificate you can comment the lines responsible for generating it.
4. Compiling and deploying client:
   1. Download detours from https://www.microsoft.com/en-us/download/details.aspx?id=52586 and extract it to the root of Kernel Whisperer
   2. Start->Microsoft Visual Studio [VS VERSION]->Visual Studio Tools->Developer Command Prompt for VS[VS VERSION]
   3. Change to Kernel Whisperer's root folder and then detours folder
   4. Run **nmake all** within the directory. Once compilation is over, change back to Kernel Whisperer's root folder
   5. Run CompileAndDeployClient.cmd. 
   

## Test environment

Kernel Whisperer has been tested with the following environment:

* Windows 7 Enterprise SP1 x64 (client + driver) + MySQL(Ver 14.14 Distrib 5.5.59) on Linux remnux 3.13.0-53-generic
