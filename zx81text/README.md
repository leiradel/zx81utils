# ZX81TEXT

When text just won't work when trying to show a ZX81 program listing, we have to make an image. Making images from text output of **AGEPLIST** and **WMAPLIST** is not too much difficult, but **ZX81TEXT** makes the process much easier. Just type in the command line:

```
$ wmalist -a program.p | zx81text -o image.bmp
```

and the listing will appear in the BMP. You don't even need to have the ZX-81.TTF font, the characters are generated from the ZX81 ROM.
