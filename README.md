# Safety-app

The ELISA enhanced AGL instrument cluster demo offers an interface using a named pipe to trigger safe state from within the safety signal source. Alternatively the communication between signal source and safety app can be corrupted. This app is used to test the implemented mechanisms. 

## Signalsource-control-panel interface

An ncurses based control panel can be used as a convenient alternative to writing to the named pipe directly.

```
Signalsource-control-panel
```

To access the control panel, log into the running QEMU instance as user "root" (no password needed) and start the control panel application.

More details to build the demo and start the qemu image with the proper parameter is provided in the meta-elisa [README](https://github.com/elisa-tech/meta-elisa).

## Bitbake recipe for the pipe demo

The matching bitbake recipe ([pipe-demo](https://github.com/elisa-tech/meta-elisa/blob/master/meta-elisa-demo/recipes-elisa/pipe-demo/)) can be found in the [meta-elisa](https://github.com/elisa-tech/meta-elisa) repository. 