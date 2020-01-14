# xinOS
xinOS is a operating system. It uses 16 bit color graphics and emulates the way NES games were played. By default, there are 2 applications loaded on it (xinSnake and xinMusic), but you can write your own in C and play them!

## How do I run it?
First off, download the xinOS image from the "releases" tab on GitHub. This is the actual image that the operating system is on

xinOS, being an operating system, can be burned onto a USB or Floppy drive and booted from (it works on ancient hardware, including the Commodore 64). Alternatively, if you'd like to emulate it, you can use the emulater "QEMU" (this is similar to VirtualBox or VMWare) or "Bochs". QEMU is the preferred emulator. To emulate it, run the following command:

```
qemu-system-i386 -fda <file path of the xinOS image here> -s -machine q35 -soundhw pcspk
```

If you've installed qemu but that qemu-system-i386 isn't found, you can run it with the generic QEMU command

```
qemu -fda ./build/xinos.img -s -machine q35 -soundhw pcspk
```

If you are running Bochs, a config is premade for you.

## I'd like to design some games for xinOS!
Awesome! xinOS provides a friendly interface for drawing pixels, squares, triangles, circles, and other geometric shapes (as well as text) onto the screen, as well as an interface for IO tasks, keyboards, and other tasks. You don't need to know a single thing about operating systems to write a game for xinOS. Though, you should know C.

As of now, there is no official documentation for making games. Right now, you should go look at the snake game as a reference, which is heavily commented. If you'd like to see fully fledged documentation, please star this project! I don't want to make documentation no one will use, so starring lets me know people want documentation to make games.