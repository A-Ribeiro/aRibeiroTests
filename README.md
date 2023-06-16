> For this is how God loved the world:  
he gave his only Son, so that everyone  
who believes in him may not perish  
but may have eternal life.  
  \
John 3:16

## The Tests

I tried to cover all major funcionalities from the libraries I developed.

I use this code to guarantee the code to behave the same way on Linux, Mac(Intel) and Windows.

## How to Clone?

This library uses git submodules.

You need to fetch the repo and the submodules also.

### a) Clone With Single Command

__HTTPS__

```bash
git clone --recurse-submodules https://github.com/A-Ribeiro/aRibeiroTests.git
```

__SSH__

```bash
git clone --recurse-submodules git@github.com:A-Ribeiro/aRibeiroTests.git
```

### b) Clone With Multiple Commands

__HTTPS__

```bash
git clone https://github.com/A-Ribeiro/aRibeiroTests.git
cd OpenGLStarter
git submodule init
git submodule update
```

__SSH__

```bash
git clone git@github.com:A-Ribeiro/aRibeiroTests.git
cd OpenGLStarter
git submodule init
git submodule update
```

### c) update only the submodules

```bash
git submodule update
```

## GitHUB Link

https://github.com/A-Ribeiro/aRibeiroCore

https://github.com/A-Ribeiro/aRibeiroPlatform

https://github.com/A-Ribeiro/aRibeiroData

https://github.com/A-Ribeiro/aRibeiroWrappers

https://github.com/A-Ribeiro/aRibeiroTests

### Tools

https://github.com/A-Ribeiro/assimp2bams

https://github.com/A-Ribeiro/font2bitmap

## Notes

### Trigger a SIGINT on XCode

Start the debug session and click the pause button.

This will interrupt all threads and open the gdb console.

Type this:

```
process handle SIGINT --stop false --pass true --notify false
```

This will let the XCode to pass the SIGINT for the custom handlers inside the code.

Now you just need to open a terminal and type:

```
killall -SIGINT test-ipc
```

This will trigger the signal to the current project you are testing.

## Authors

***Alessandro Ribeiro da Silva*** obtained his Bachelor's degree in Computer Science from Pontifical Catholic 
University of Minas Gerais and a Master's degree in Computer Science from the Federal University of Minas Gerais, 
in 2005 and 2008 respectively. He taught at PUC and UFMG as a substitute/assistant professor in the courses 
of Digital Arts, Computer Science, Computer Engineering and Digital Games. He have work experience with interactive
software. He worked with OpenGL, post-processing, out-of-core rendering, Unity3D and game consoles. Today 
he work with freelance projects related to Computer Graphics, Virtual Reality, Augmented Reality, WebGL, web server 
and mobile apps (andoid/iOS).

More information on: https://alessandroribeiro.thegeneralsolution.com/en/
