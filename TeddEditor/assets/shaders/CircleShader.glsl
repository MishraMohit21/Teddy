//--------------------------
// Renderer2D Circle Shader (Updated)
// --------------------------

#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_Thickness;
layout(location = 3) in float a_Fade;
layout(location = 4) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_LocalPosition;
out float v_Thickness;
out float v_Fade;
out flat int v_EntityID;

void main()
{
    // Calculate local position based on a unit square
    vec2 localPos = vec2(0.0);
    if (gl_VertexID == 0) localPos = vec2(-1.0, -1.0);
    else if (gl_VertexID == 1) localPos = vec2(1.0, -1.0);
    else if (gl_VertexID == 2) localPos = vec2(1.0, 1.0);
    else if (gl_VertexID == 3) localPos = vec2(-1.0, 1.0);

    v_Color = a_Color;
    v_LocalPosition = localPos;
    v_Thickness = a_Thickness;
    v_Fade = a_Fade;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Color;
in vec2 v_LocalPosition;
in float v_Thickness;
in float v_Fade;
in flat int v_EntityID;

void main()
{
    // Calculate distance from the center (0, 0)
    float distance = 1.0 - length(v_LocalPosition);
    
    // Create smooth circle with thickness and fade
    float circle = smoothstep(0.0, v_Fade, distance);
    circle *= smoothstep(v_Thickness + v_Fade, v_Thickness, distance);
    
    // Discard if outside the circle
    if (circle == 0.0)
        discard;
    
    // Set output color with alpha
    color = v_Color;
    color.a *= circle;
    
    // Output entity ID
    color2 = v_EntityID;
}