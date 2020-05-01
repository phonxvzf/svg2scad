# svg2scad
A simple program to generate OpenSCAD model from an SVG image.

## Prerequisites
- C++14 compliant compiler
- CMake

## Building
```shell
$ cmake .
$ make
```

## Running
```
$ svg2scad [OPTIONS] SVGFILE
```

where available options are
- `-o, --output` Specify OpenSCAD output file name with extension
- `-m, --modname` Specify module name to be generated. `svg_generated` will be used if this option is not specified.
- `-s, --segments` Specify number of line segments for each curve
- `-h, --help` Print help text and exit with failure

### Example
To generate OpenSCAD file:
```
svg2scad gnu.svg -o gnu.scad --modname gnu -s 3
```
Then, use it like the usual way in your another OpenSCAD file:
```scad
use <gnu.scad>
gnu(2, 5); // 2 is line thickness and 5 is the object's depth (Z axis)
```

## License
This software is released under MIT license.
