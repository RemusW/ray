read -p "Scene name: " sceneN
read -p "recursions: " recur
cd ..
make -j8
cd bin/
./ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/public_scenes_part1/scenes/$sceneN.ray /home/remusw/Documents/Graphics/Proj1/ray/build/bin/renders/temp.png -r $recur