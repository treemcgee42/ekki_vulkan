
## Queues

We only create graphics and present queues, as there is no real need for any others.

## Render pass design

### Attachments

Besides drawing an image to the screen, we want to have order-independent display of objects on the screen with regards to which is on top. Therefore, we need

* *color attachment*: to draw the image to
* *depth attachment*: to keep track of which fragment is on top
