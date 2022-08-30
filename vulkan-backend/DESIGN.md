
## Queues

We only create graphics and present queues, as there is no real need for any others.

## Swap chain

In this situation, the swap chain images are the ones we want to render to and display. Image views are 2D, 

## Render pass design

### Attachments

Besides drawing an image to the screen, we want to have order-independent display of objects on the screen with regards to which is on top. Therefore, we need

* *color attachment*: to draw the image to
* *depth attachment*: to keep track of which fragment is on top

## Render pass vs. renderer vs. render system

```
... --> render pass --> renderer --> render system
```

The render pass was responsible for creating the render pass and framebuffers, which go hand in hand as they describe the kind of thing we are going to push through the graphics pipeline. But again, nothing is actually being pushed through.

The render system sets is what actually gives the commands to the GPU. This means it needs to set up the graphics pipeline and push commands through.

We want the render system to only worry about rendering. It shouldn't worry about which frame in the swap chain to use, for example. That is the job of the renderer. It handles all the synchronization. So every frame you start with the renderer, which will give a command buffer for the render system to use, and when the render system is done recording commands the renderer will step back in and handle the synchronizing of submitting this to the GPU.

One goal of this design is that there can potentially be several render systems depending on what it is you are trying to render (changing shaders is an example).

```
renderer
---- render system 1
---- render system 2
```

In this project, synchronization happens at acquisition.