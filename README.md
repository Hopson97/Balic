# Balic

Bad and Lossy Image Compressor

A program that "compresses" the quality of an image, allowing you view the process while it happens.

## Building

Requires SFML library

On Ubuntu:

```
sudo apt-get install libsfml-dev
```

Building:

```
git clone https://github.com/Hopson97/Pixelator.git
cd Balic
cmake .
make
```

## Running

```
./balic <image-file> <output>
```

Example:
```
./balic chinatown.jpg out.jpg
```


## Example



![ChinaTown](https://wp.zillowstatic.com/streeteasy/2/Chinatown-Madhu-Nair-flickr.jpg =300x242) ![ChinaTownBalic](https://i.imgur.com/tAchXKD.gif =300x242)


Image source: https://streeteasy.com/blog/chinatown-still-nycs-best-real-estate-bargain/