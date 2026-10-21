# acwake

Wakes the laptop from suspend-to-idle when the AC adapter is plugged in.
Written for a ThinkPad 21ME (P14s Gen 5 AMD) on CachyOS, kernel 7.2.

## Build and test once

    make
    sudo insmod acwake.ko

## Install permanently (survives kernel updates)

    sudo pacman -S --needed dkms
    sudo cp -r ${WORKDIR}/acwake /usr/src/acwake-1.0
    sudo dkms install acwake/1.0
    echo acwake | sudo tee /etc/modules-load.d/acwake.conf

## Controls

    echo disabled | sudo tee /sys/class/power_supply/AC/power/wakeup   # off until reboot
    echo enabled  | sudo tee /sys/class/power_supply/AC/power/wakeup   # back on
    echo Y | sudo tee /sys/module/acwake/parameters/wake_on_unplug
