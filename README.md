# ekki
**an educational animation library**

This project was inspired by my particular use of the `manim` library. Making videos primarily on abstract, pure math, I found that I didn't utilize many of the features in `manim`, such as graphing. I mostly used the ability to present text and draw shapes. While `manim` is incredible, there were a few drawbacks I'd found in my use of it:
* *cumbersome to edit*— the programmatic interface is useful for creating complex visualizations, but I wasn't doing that, and it felt like things could be going so much faster if I could just edit the scene in a GUI.
* *slow to render*— rendering is done offline and compiled into a video file, which takes very long, even for relatively simple scenes.
* *weak 3D support*— again specific to my use case, I want to be able to create scenes with 3D geometry. Here a GUI would go a long way. Currently, my workaround is to model the geometry in another software and export it as an image.

`ekki` aims to solve this by providing a GUI for real-time edits and fast rendering. 

## Roadmap
There are three main milestones for this project:
* `eklib` (short term, in progress): the "ekki library", a C++ library that can be used to create animations programmatically. In addition, this should be the primary interface for the subsequent milestones. Because of this, `eklib` will have to be continually improved upon. 
* `ek2d` (medium term): a standalone GUI for creating 2D animations
* `ek3d` (long term): a standalone GUI for creating 3D animations

## FAQs

Licensing, contributing?
* there is no license, which means this is under exclusive copyright by default, and *technically* no one else has permission to use, modify, or share this. Reach out to me (varun.malladi@gmail.com) if you're interested in this changing; I'm extremely open to the idea.

What technology is used?
* the project is written primarily in C++, and utilizes the Vulkan API for the graphics backend. The GUI is made with `imgui`.

Why not use Rust 🦀?
* hey, I love Rust as much as the next guy, but one of my personal goals with this project was to learn graphics programming through Vulkan, and doing that in C++ is just easier, and also good practice since it's the industry standard.
