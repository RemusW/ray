#!/bin/bash
cd ..
make -j8
cd bin/
./ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/public_scenes_part1/scenes/$1.ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/renders/temp.png -r $2
./ray-solution /home/remusw/Documents/Graphics/Proj1/ray/build/bin/public_scenes_part1/scenes/$1.ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/renders/sol.png -r $2
