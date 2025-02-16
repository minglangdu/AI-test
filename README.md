# AI evolution

Since a previous project didn't work so well, I'm now going to try out AI! 

This code is written mostly from scratch with snippets borrowed from LazyFoo's [tutorial](https://lazyfoo.net/tutorials/SDL/index.php) on SDL. 

## Debug

If you enable `DEBUG_WIND` in `constants.h`, a separate debug window will appear showing the network inside one of the agents. 

The values are symbolized with a color between red and green, where red is 0 and green is 1. 
The outline around each node is its bias, and the filled square inside is its value. 
The edge's color symbolizes its effect on the node it feeds into.

## Credits

I have included several folders on this repository purely so that I don't have to go through the install process again if I want to develop on another computer. 
These folders' creators are linked below: 

- [SDL2](https://www.libsdl.org/)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)