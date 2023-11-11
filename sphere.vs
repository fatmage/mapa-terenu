#version 330 core

layout (location = 0) in float height;

uniform float lon;
uniform float lat;
uniform mat4 view;
uniform mat4 projection;

float earth_radius = 6371.0;
out float ht;

void main() {

    vec2 span = vec2(0.0, 0.0);



    float lonc = -lon;
    float latc = lat;

    float radius;
    
    if (height < -500)
        radius = earth_radius;
    else 
        radius = earth_radius + (height/200);

    float londisp;
    if (lon < 180)
        londisp = mod(gl_VertexID, 1201)/1200;
    else
        londisp = (1200-mod(gl_VertexID, 1201))/1200;

    float latdisp;
    if (lat < 90)
        latdisp = (1200 - floor(gl_VertexID/1201.0))/1200;
    else
        latdisp = floor(gl_VertexID/1201.0)/1200;


    if (lon < 0)
        lonc = lonc - londisp;
    else 
        lonc = lonc + londisp;

    if (lat < 0)
        latc = latc - latdisp;
    else 
        latc = latc + latdisp;


    // latem po igreku
    // lonem po iksie

    lonc = radians(lonc);
    latc = radians(latc);




    vec3 world_pos = vec3(radius,0.0, 0.0);

    mat3 rot_x = mat3(1.0,       0.0,        0.0,
                  0.0, cos(lonc), -sin(lonc),
                  0.0, sin(lonc),  cos(lonc));

    mat3 rot_y =  mat3(cos(latc), 0.0, sin(latc),
                         0.0, 1.0,       0.0,
                  -sin(latc), 0.0, cos(latc));

    rot_x = transpose(rot_x);
    rot_y = transpose(rot_y);

    world_pos = rot_y * world_pos;
    world_pos = rot_x * world_pos;



    ht = height;
    gl_Position = projection * view * vec4(world_pos, 1.0);
}

