# Gyacht
Gyacht is a GUI based container and image viewer program for unprivileged container cli tools 
such as Podman, Buildah and so on in which edit {containers,images}.json files.
Currently, it only prints the list of containers and images.

One thing this is cool (I guess ;-D) is that the program is monitoring the changes of the containers 
and images, and therefore it updates the list box of both immediately.

*Note that errors will be occurred if you haven't installed one of both, Podman and Buildah yet.*

Demo: https://youtu.be/T1h_aqfVagw

### Quick installation with flatpak
$ git clone https://github.com/yisooan/gyacht.git  
$ cd gyacht/flatpak  
$ flatpak-builder ./_build com.github.yisooan.gyacht.json  

### Run with flatpak
$ flatpak-builder --run ./_build com.github.yisooan.gyacht.json gyacht  

### Print debug messages with flatpak
$ flatpak-builder --run ./_build com.github.yisooan.gyacht.json gyacht-devel.sh  

### Do you have ideas for the project?
Feel free to make issues :-)  
