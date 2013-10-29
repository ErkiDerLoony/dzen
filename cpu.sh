#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: ./cpu.sh LENGTH" >&2
    exit 1
fi

length="$1"

line=$(head -n1 /proc/stat | sed 's/cpu *//')
old_user=$(echo "$line" | cut -d ' ' -f 1)
old_nice=$(echo "$line" | cut -d ' ' -f 2)
old_system=$(echo "$line" | cut -d ' ' -f 3)
old_idle=$(echo "$line" | cut -d ' ' -f 4)
old_iowait=$(echo "$line" | cut -d ' ' -f 5)
old_irq=$(echo "$line" | cut -d ' ' -f 6)
old_softirq=$(echo "$line" | cut -d ' ' -f 7)
old_steal=$(echo "$line" | cut -d ' ' -f 8)
old_guest=$(echo "$line" | cut -d ' ' -f 9)
old_guest_nice=$(echo "$line" | cut -d ' ' -f 10)

while [ 1 ]; do
    sleep 1
    line=$(head -n1 /proc/stat | sed 's/cpu *//')
    user=$(echo "$line" | cut -d ' ' -f 1)
    nice=$(echo "$line" | cut -d ' ' -f 2)
    system=$(echo "$line" | cut -d ' ' -f 3)
    idle=$(echo "$line" | cut -d ' ' -f 4)
    iowait=$(echo "$line" | cut -d ' ' -f 5)
    irq=$(echo "$line" | cut -d ' ' -f 6)
    softirq=$(echo "$line" | cut -d ' ' -f 7)
    steal=$(echo "$line" | cut -d ' ' -f 8)
    guest=$(echo "$line" | cut -d ' ' -f 9)
    guest_nice=$(echo "$line" | cut -d ' ' -f 10)
    diff_user=$(($user-$old_user))
    diff_nice=$(($nice-$old_nice))
    diff_system=$(($system-$old_system))
    diff_idle=$(($idle-$old_idle))
    diff_iowait=$(($iowait-$old_iowait))
    diff_irq=$(($irq-$old_irq))
    diff_softirq=$(($softirq-$old_softirq))
    diff_steal=$(($steal-$old_steal))
    diff_guest=$(($guest-$old_guest))
    diff_guest_nice=$(($guest_nice-$old_guest_nice))
    sum=$(($diff_user+$diff_nice+$diff_system+$diff_idle+$diff_iowait+$diff_irq+$diff_softirq+$diff_steal+$diff_guest+$diff_guest_nice))
    scaled_user=$(awk "BEGIN{print int(0.5+${length}*${diff_user}/$sum)}")
    scaled_nice=$(awk "BEGIN{print int(0.5+${length}*${diff_nice}/$sum)}")
    scaled_system=$(awk "BEGIN{print int(0.5+${length}*${diff_system}/$sum)}")
    scaled_idle=$(awk "BEGIN{print int(0.5+${length}*${diff_idle}/$sum)}")
    scaled_iowait=$(awk "BEGIN{print int(0.5+${length}*${diff_iowait}/$sum)}")
    scaled_irq=$(awk "BEGIN{print int(0.5+${length}*${diff_irq}/$sum)}")
    scaled_softirq=$(awk "BEGIN{print int(0.5+${length}*${diff_softirq}/$sum)}")
    scaled_steal=$(awk "BEGIN{print int(0.5+${length}*${diff_steal}/$sum)}")
    scaled_guest=$(awk "BEGIN{print int(0.5+${length}*${diff_guest}/$sum)}")
    scaled_guest_nice=$(awk "BEGIN{print int(0.5+${length}*${diff_guest_nice}/$sum)}")
    corrected_idle=$(($length-$scaled_user-$scaled_system-$scaled_nice-$scaled_iowait-$scaled_irq-$scaled_softirq-$scaled_steal-$scaled_guest-$scaled_guest_nice))
    old_user="$user"
    old_nice="$nice"
    old_system="$system"
    old_idle="$idle"
    old_iowait="$iowait"
    old_irq="$irq"
    old_softirq="$softirq"
    old_steal="$steal"
    old_guest="$guest"
    old_guest_nice="$guest_nice"
    echo "^fg(blue)^r(${scaled_nice}x10)^fg(green)^r(${scaled_user}x10)^fg(red)^r(${scaled_system}x10)^fg(grey)^r(${scaled_iowait}x10)^r(${scaled_irq}x10)^r(${scaled_softirq}x10)^fg(cyan)^r(${scaled_steal}x10)^r(${scaled_guest}x10)^r(${scaled_guest_nice}x10)^fg(darkgrey)^r(${corrected_idle}x10)"
done
