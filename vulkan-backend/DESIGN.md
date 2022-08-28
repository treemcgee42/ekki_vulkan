
```
Window --> Device --> Renderer (--> Render system)
```

## Window

We use GLFW to handle window creation, which is basically a matter of calling the right operating system API. Since Vulkan does not need a window to run, and indeed neither do most graphics APIs, it does provide a way to handle this.

Before connecting this up to Vulkan, we should consider what information GLFW has and what will be sent to GLFW during our program's lifecycle, so that we know what we need to redirect to our engine.

- `GLFWwindow*`: this is the pointer to the GLFW abstraction of the window.
- *window resizing*: whenever the window is resized by the user, the first one to know will be GLFW. We can actually tell GLFW to call a specific function (which we provide) whenever this occurs.

In terms of the sequential order of things, this is all that needs to happen from the GLFW side of things. Future steps will, however, use the data from this step. We need to connect Vulkan to the window, after all.

## Device

### Instance creation

To be able to use Vulkan, we need to create an instance of it. This is analogous to how you may need to create an instance of a class in order to use it. One might imagine this is necessary because Vulkan makes no assumption on "sensible defaults", and therefore must be told how it must be configured.

In particular, this is where you tell what *global layers* you would like to enable. (TODO: better description of what these are.) In our case, we would like to enable whatever layers we need to connect with GLFW, and maybe even some portability layers that will allow for the code to run on, say, MacOS.

### Debug messenger

This would be a good time to set up the debug messenger, if you would desire it. This is basically hooking up a function you yourself to the Vulkan instance, which Vulkan will use to write more detailed debugging information.

### Vulkan window surface

Now that you have an instance, you can start setting up Vulkan. One of the things we need to do is connect Vulkan to the GLFW window. This amounts to taking GLFW's abstraction of a "window" and creating a corresponding Vulkan abstraction of that same "window". This is necessary because Vulkan doesn't assume you are using GLFW, and GLFW doesn't assume you are using Vulkan, so you need to tell Vulkan how to interpret the window you created with GLFW. Fortunately, GLFW itself provides a function that gives you the Vulkan abstraction of its window.

### Physical device

Now you need to tell Vulkan what physical device (e.g. GPU) you want to use. In fact, you could choose multiple. Vulkan can literally give you a list of available devices, and it's up to you to pick the one you want to use. You should check that a device supports all the features you want before picking it.

### Logical device

After having picked a physical device, we need to create what is called a *logical device*. While at first this might just seem like a simple wrapper around the physical device, it is actually significantly different. There are two main differences compared to the physical device:

* describes what features of the physical device we will be using
* creates whatever *queues* we tell it to

You can think of the logical device is the object that we configure and reconfigure throughout the lifecycle of the program. If we want something from the GPU, we ask the logical device. 

#### Queues, and how we do things in Vulkan

Whenever we tell Vulkan to do something, which by proxy means whenever we tell the GPU to do something, we submit a *command*. We say "submit" because commands are not simply executed right when you call them in your code. After all, your code is running on the CPU but you are trying to do work on the GPU. The CPU has no idea how the GPU is scheduling its tasks.

You need to submit your command to a *command queue*. These are chunks of memory on the GPU itself (TODO: ?) that store commands. Now, you might ask the GPU to do different tasks of very different natures. You want to draw something, use a *graphics queue*. You want to compute something, use a *compute queue*. 

What does it mean to "submit" a command? If command queues are in GPU memory, how do we put our command there? Usually, we create a *command buffer*, which is a buffer of memory we can put one or several commands in. We can then tell the queue to add the address of this buffer (TODO: ?).

So there are two questions you might have. 

* Why are there queues that only do specific commands, like compute queues?

As hinted at above, GPUs often support queues that can handle multiple kinds of commands, such as graphics and compute commands. However, they may also have dedicated queues for certain tasks, like computing. This allows the GPUs to make certain optimizations, which is fascinating but beyond our scope at present. A *queue family* tells us what kind of commands can go into a queue. For example, our GPU might support up to 8 queues under the queue family that solely supports compute commands.

Note the subtlety here. A graphics and present queue can (and often are) be in the same queue *family*, but will always be different queues.

Back to Vulkan, when we create our logical device we need to also create the queues we want. For the purposes of this program, we really only need a graphics queue and a *present queue*. A present queue takes the commands that actually print the image we create to the screen, whereas the graphics queue takes draw commands, which actually just calculate the pixels of the image (TODO: ?).

### Command pool

A command pool is a large collection of memory on the GPU (TODO: details?) from which we can allocate our command buffers. The idea behind it is that allocating memory is an expensive operation, so it wouldn't be efficient to try and find memory every time we needed to submit a command (several times every frame). Instead, we can allocate a bigger chunk of memory and just keep reusing it.

## Renderer

### Creating the swap chain

Let's assume you already know what a swap chain is. Since it is what is going to be displayed, you will need to tell it about a bunch of the window and device properties. 

Ultimately though, it will provide us with a collection of *images* that our queues can use (and their commands can operate on). Commonly, we will have a graphics queue and a present queue. The graphics queue will draw to an image, and the present queue will present an image.

To create the swap chain, Vulkan needs to know what kinds of commands to expect so that it can be configured accordingly. Since there is concurrency going on, the issue is (TODO: clarify why we don't need concurrency within a queue family). Recall that this is essentially what a queue family tells us. Assuming our situation with two queues, graphics and present, there are two cases:

* *they are in same queue family*. In this case, since we know that no other queue family will need to access the swap chain images, we can specify `VK_SHARING_MODE_EXCLUSIVE`, which disallows a queue from another queue family to use this image (without explicit ownership transfer).
* *they are in different queue families*. You need `VK_SHARING_MODE_CONCURRENT` to indicate that ownership does not need to by explicitly transferred for this image to be used by separate queue families.

### Recreating the swap chain

You also want to handle the case when you need to recreate the swap chain, which can happen if the window is resized or moved to a different monitor, for example.

This is pretty much the same as creating, but one interesting thing you can do is actually provide the old swap chain to the Vulkan API function creating the new one. According to the specification, this may aid in resource reuse, but crucially it also allows the application to still present any images already aquired from it. This would make it seem less like the display was "freezing" when you are recreating the swap chain.

### Creating command buffers

You may also create your command buffers here. You might ask, how will we know how many command buffers to create at the point of our engine's initialization? It's a fair point, and you may choose to do it differently. But one approach is to have one command buffer for each image in your swap chain. Really, this is as "coarse" as you can go— it wouldn't make sense to share one command buffer for multiple images in the swap chain since you can only display one frame and the command buffer would be popped off the queue afterwards. You could have multiple command buffers per swap chain image, though.

The existence of a command pool is to optimize cases when you may need to create command buffers more often than just at initialization.

## Render pipeline

At this point, we are done with initialization. Your window is up and running. Your swap chain is in play. Everything from now on can be changed by your program during execution. For example, you can switch to a different vertex shader.

The remaining thing to do is set up the graphics pipeline.

### Shaders

Your shaders are, by definition, programming for the programmable parts of the graphics pipeline. So you need to tell your pipeline about them.

### Render pass

Rendering commands need to happen inside of a render pass. The render pass is essentially providing the objects that the rendering commands will draw to. This could be as simple as just an image containing colors for the final output. You will certainly need at least this. But it could also contain other images that help calculate that final color image. For example, you could have a depth image (depth buffer), which keeps track of which fragment is closest to the viewer, and hence being displayed. Then, every time Vulkan was going to write to the color image, it would check that fragment's depth value against the depth image to see if it really should be overriding whatever fragment is already there. If you didn't do this, whatever overlapping shapes are visible would be whichever ones were drawn last.

The render pass doesn't actually hold the images (technically image views), though. It merely describes what ones are there. The kind of image view is called an *attachment*. What actually holds the image views is...

### Framebuffer

This holds the image views. Think of it as a chunk of memory, that we are putting other chunks of memory (the image views) in. To be able to make any sense of the frame buffer memory, we need the render pass, which will tell us what kind of image view is in what order. For instance, a frame buffer with a `{ color view, depth view }` should be coupled with a render pass saying `{ color attachment, depth attachment }`. 

The framebuffer is what goes through the graphics pipeline and gets acting on by our commands.



 
