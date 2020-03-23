# API server for vacuum pressure monitor for Pfeiffer IF300A

This program is based on Oat++(https://oatpp.io/) and MongoDB and (ROOT).  The monitor program https://github.com/aogaki/VacMonitor is working with libserial and mongocxx driver.  I simply use the libserial provided and installed Ubuntu 18.04.  In some documents, class member function and variables are not same.  When you will run on another distribution and/or another version, please be careful and modify the code.

## Overview

This server is only API server (and a little bit data analysis by ROOT).  The data taking and storing into MongoDB is done by VacMonitor(https://github.com/aogaki/VacMonitor).  
VacMonViewer(https://github.com/aogaki/VacMonViewer) is a web page application to watch the graph pressure vs time.  The viewer is written by TypeScript with Angular.  The web page files (html, js, css) are generated and store VacMonServer/Monitor.  In the case of you will modify and update, please google how to deploy Angular.  Now the base directory name of the server is vacmon.  When you regenerate the files, please do not forget to use --base-href=/vacmon/ option.

#### Using CMake

**Requires** 

- `oatpp` module installed. You may run `utility/install-oatpp-modules.sh` 
script to install required oatpp modules.

```
$ mkdir build && cd build
$ cmake ..
$ make 
$ ./my-project-exe  # - run application.
