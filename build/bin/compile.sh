#!/bin/bash
cd ..
make -j8
cd bin/
./ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/public_scenes_part1/scenes/$2.ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/renders/temp.png -r $1
./raysol-linux /home/remusw/Documents/Graphics/Proj1/ray/build/bin/public_scenes_part1/scenes/$2.ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/renders/sol.png -r $1
