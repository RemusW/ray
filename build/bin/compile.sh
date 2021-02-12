read -p "Scene name: " sceneN
cd ..
make -j8
cd bin/
./ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/public_scenes_part1/scenes/$sceneN.ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/renders/temp.png