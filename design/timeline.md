# timeline design

## the data structure

The base structure is a dynamic array with an element for each second of the scene. The element is a linked list of `Animation`s that are active _during_ that second. It includes, for instance, animations that begin half a second after that second.

## usage

During playback, the system should know at every frame how far the scene has progressed. This time is rounded down to the nearest second, and the active animations are determined by going through the corresponding linked list. 

This method is exactly the same for jumping to any point in the scene.

## justification

A robust playback system should be able to determine which animations are active at any point in time. The goal of this design was to avoid having to check all animations in the scene to determine which were active. 

A few assumptions were in play. First, we assume that most scenes are relatively short. This assumption, and its specifics, is relevant because it determines how much memory the timeline requires. Another assumption is that most animations begin and end at second intervals. If a large number of animations didn't, it would be needlessly expensive to find the active animations at a frame during that second, since potentially many of the animations in that second are not active during that frame.

## potential optimizations

- (adjusting the granularity). The 1-second intervals in the structure may not be the most appropriate for the scene, for example if many animations begin and end on (non-second) half-second intervals. A user could be able to adjust the structure to store active animations for every half-second, or this could be dynamically decided by the program. The cost of this optimization is memory.
- (combining animations). If several animations begin and end at the same time, they can be grouped together into a single animation. The user could do this themselves, but even in that case it might be best for the program to do this itself and hide that abstraction from the user. This would really only be worth it if there were a large number of animations beginning and ending at the same times, for example if the user manually animated a large number of particles. 