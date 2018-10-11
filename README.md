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


<span><img src="https://wp.zillowstatic.com/streeteasy/2/Chinatown-Madhu-Nair-flickr.jpg" width="300" height="242"></span><span><img src="https://i.imgur.com/tAchXKD.gif " width="300" height="242"></span>

Fullsize:

![Balic](https://i.imgur.com/tAchXKD.gif)


Image source: https://streeteasy.com/blog/chinatown-still-nycs-best-real-estate-bargain/