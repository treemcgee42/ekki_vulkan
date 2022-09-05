# Animation design

A collection of `Animation`s is precisely what makes up a scene. Movement and even just displaying something is done by creating an animation.

During each frame, the program determine which animations are _active_. An active animation is one that is in progress at that time, i.e. starts before that frame and ends after. The actual frame should be able to be constructed from just the collection of active animations. 

Because the user should be able to jump to any frame, play the scene, replay the scene, etc. an animation object should not be mutated during playback. Instead, an (active) animation should be able to take the time of the frame in the scene and determine what object should be displayed and where.