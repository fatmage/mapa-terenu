#version 330 core

layout (location = 0) in vec2 disp;

uniform float lon;
uniform float lat;
uniform mat4 view;
uniform mat4 projection;


float earth_radius = 6371.0;


void main() {

    float lonc;
    float latc;



    //if (lon < 0)
    //    lonc = lon - disp.x;
    //else 
    //    lonc = lon + disp.x;

    //if (lat < 0)
    //    latc = lat - disp.y;
    //else 
    //    latc = lat + disp.y;

    lonc = -lon + -disp.x;
    latc = lat + disp.y;


    // latem po igreku
    // lonem po iksie

    lonc = radians(lonc);
    latc = radians(latc);


    vec3 world_pos = vec3(earth_radius, 0.0, 0.0);

    mat3 rot_x = mat3(1.0, 0.0,        0.0,
                  0.0, cos(lonc), -sin(lonc),
                  0.0, sin(lonc),  cos(lonc));

    mat3 rot_y =  mat3(cos(latc), 0.0, sin(latc),
                         0.0, 1.0,       0.0,
                  -sin(latc), 0.0, cos(latc));

    rot_x = transpose(rot_x);
    rot_y = transpose(rot_y);

    world_pos = rot_y * world_pos;
    world_pos = rot_x * world_pos;




    gl_Position = projection * view * vec4(world_pos, 1.0);
}

