GLSLC="/Users/ogmalladii/VulkanSDK/1.3.216.0/macOS/bin/glslc"
SHADERS_DIR=shaders

$GLSLC $SHADERS_DIR/simple_shader.vert -o $SHADERS_DIR/simple_shader.vert.spv
$GLSLC $SHADERS_DIR/simple_shader.frag -o $SHADERS_DIR/simple_shader.frag.spv