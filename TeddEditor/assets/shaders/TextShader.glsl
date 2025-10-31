#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout (location = 0) out vec4 v_Color;
layout (location = 1) out vec2 v_TexCoord;
layout (location = 2) out flat int v_EntityID;
layout (location = 3) out flat float v_TexIndex;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoord;
layout (location = 2) in flat int v_EntityID;
layout (location = 3) in flat float v_TexIndex;

layout(binding = 0) uniform sampler2D u_FontAtlases[32];

void main()
{
    // Sample the SDF value from the correct font atlas
    float distance = 1.0 - texture(u_FontAtlases[int(v_TexIndex)], v_TexCoord).a;

    // Use screen-space derivative to calculate a dynamic smoothing range.
    float width = fwidth(distance);

    // Smoothstep to transition from outside (alpha=0) to inside (alpha=1).
    float alpha = smoothstep(0.5 - width, 0.5 + width, distance);

    // Combine with vertex color and apply final alpha.
    o_Color = vec4(v_Color.rgb, v_Color.a * alpha);
    
    // Pass entity ID through for editor picking.
    o_EntityID = v_EntityID;

    // Discard fully transparent fragments for a minor performance gain.
    if (o_Color.a < 0.01)
        discard;
}