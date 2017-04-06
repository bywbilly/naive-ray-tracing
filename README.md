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

	- `--width = [int] default = 640`

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
