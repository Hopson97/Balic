# Balic

Bad and Lossy Image Compressor

A program that "compresses" the quality of an image, allowing you view the process while it happens.

It does this by looking at pixel, and then doing a recursive flood fill until it cannot find anymore pixels of a similar colour. This continues until the entire image has been "compressed".

This program does actually compress the image, it is just a name for a joke, as image compressing usually results in loss of image quality.

## Building

Requires SFML library

On Ubuntu:

```
sudo apt-get install libsfml-dev
```

Building:

```
git clone https://github.com/Hopson97/Balic.git
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

Flood fill compress:

<span><img src="https://wp.zillowstatic.com/streeteasy/2/Chinatown-Madhu-Nair-flickr.jpg" width="400" height="342"></span><span><img src="https://i.imgur.com/tAchXKD.gif " width="400" height="342"></span>

Fullsize:

![Balic](https://i.imgur.com/tAchXKD.gif)

Line Compression:

![china_town_lines](https://i.imgur.com/JVhB3An.jpg)




Image source: https://streeteasy.com/blog/chinatown-still-nycs-best-real-estate-bargain/

