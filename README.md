# OpenSpace Auton
Created by Paul Bailey at HMNS in 2022

This is a tool that allows the creation of OpenSpace recording files from XML defined paths and commands. Its intent is to allow for the creation of precise, repeatable recordings without external command.
An input file at "PROJECTDIR"/res/Auton.xml is read in and a recording is output to C:\OpenSpace\OpenSpace-0.18.0\user\recordings\

An auton is defined by the xml root element "auton"
auton:
- name: name of recording to be output
- speed: default speed of movement
- simTime: The inital simulation time in j2000

Commands that may be used inside the "auton" element include all of the following:

move: (latitude longitude altitude) Indicates a camera move to the specified location on earth
- look: the calculation that should be used to compute the view direction. This may be "target" (looks towards last lookTarget), "forward" (looks towards next point), or "backward" (looks towards last point)
- speed: the speed that should be maintained in that segment
- altSpeed: boolean as to whether to increase the speed with elevation
- factor: number of interpolations to preform in that segment (1 doubles the points, 2 quadruples...). The interpolations are done geospacially so that altitude is maintained. Long segments require this because OpenSpaces interpolation is done in the coordinate space.
- delay: number of seconds to pause. The point that the delayed must have repeated move lines. (Note: bug for repeated lines with look forward)

coordinates: (a list of coordinates in the form "longitude,latitude,altitude long...") this intended to allow for a coordinate path from a Google Earth kml to be copy/pasted
- Has all of the properties of move except delay
- rise: altitude to rise with every point

lookTarget: (latitude longitude altitude) sets the target point to look at for following point that look is "target"

simSpeed:
	deltaSpeed: sets the speed of the simulation in seconds per second
