#!/bin/bash


# This is simple manual DayZ launcher.
# You have to install mods trough steam workshop which the server you want to join uses.
# Then add the mod ids into 'MODS' array


SERVER_ADDR="192.168.1.141"
SERVER_PORT="2306"


MODS=(
    "1559212036" 
    "2545327648"
    "2563233742"
    "2572331007"
    "3031784065"
    "2394804821"
    "2591370539"
)


WORKSHOP="$HOME/.local/share/Steam/steamapps/workshop/content/221100/"
DAYZ="$HOME/.local/share/Steam/steamapps/common/DayZ/"

MOD_STR="-mod='"

# Reset mod symlinks, they may have been modified.
rm -r ~/.local/share/Steam/steamapps/common/DayZ/\@* 2> /dev/null

# Link mods.
for mod_id in ${MODS[@]}; do
    ln -sr $WORKSHOP$mod_id $DAYZ@$mod_id
    MOD_STR="${MOD_STR}@$mod_id;"
done

MOD_STR=${MOD_STR::-1}
MOD_STR="${MOD_STR}'"

echo $MOD_STR

steam -applaunch 221100 -nolauncher $MOD_STR #-connect=$SERVER_ADDR -port=$SERVER_PORT


