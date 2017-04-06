# naive-ray-tracing
A naive ray tracing for VR final project

## Requirements
- opencv 2.4.2
- This project is build under g++ 4.9.
- Jsonxx
- OptionParse

where `Jsonxx` is used to load json file decribing the scene and `OptionParse` is used to handle the project options. And I have already provided the source code in the project.

## Usage
`Make`

`./ray-tracing input_scene.json output_pic.png [options list]`

Here is the option args:

- `--width  = [int] default = 640`
- `--height = [int] default = 480`
- `--distance = default = 10000  where define the distance between the viewpoint and the picture`
- `--epsion = [float] default = 0.01 stop tracing the ray whose intensity reaches or go above that epsion`
- `--soft = [int] default = 0 whether use softshadow, 1 means use and 0 means not use`
- `--radius = [int] default = 4 the radius of the sampling sphere`
- `--ratio = [double] default = 0.75 the sampling rate`
- `--diffuse_num = [int] default = 0 the number of diffused ray, it may cost too much reandering time if too large`

I have implementd soft-shadow, transparency, half-transparency and color-bleeding.

I have also implementd the gouraud shading, which makes the objects more vivid than flat shading.

Here is some pictures for display.

## Softshadow

![](1.png "without softshadow")

![](14.png "with softshadow")

Also, softshadow is really expensice to render.

## transparency, half-transparency

## color-bleeding

## gouraud shading
