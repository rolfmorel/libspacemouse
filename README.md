libspacemouse
==================

A free software driver library for 3D/6DoF input devices.

Build
-----

    make
    sudo make install

Build examples
--------------

    make examples

Dependencies
------------

* libudev
    * udev deamon, `udevd`, to actually generate the connect/disconnect events
* Linux kernel's `evdev` module. This module is distributed with all major distributions.
