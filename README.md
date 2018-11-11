# RemoteProgrammer
Remote programming and reconfiguration system with LWM2M for embedded devices

## Installation

## Usage
### LwM2M server
To be able to use Remote Programmer you need a lwm2m server, we recommend using leshan with our custom object. To run this type:
``` bash
$ wget https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-server-demo.jar
$ wget https://gist.githubusercontent.com/maxiwoj/a60b566fd7fa6aec3a6e28f50640e631/raw/901798e0331a6429df3966f5616eb755b4c56098/31025.xml
$ java -jar ./leshan-server-demo.jar -m .
```
This will download a demo leshan server and our custom model definition. The flag -m specifies custom models directory location.
