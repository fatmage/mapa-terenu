#version 330 core

// Ouput data

out vec4 color;

in float ht;


void main()
{
    vec3 col;


    if      (ht < 0  )   col = vec3(0.,       0.,        1.); //blue
    else if (ht < 500)   col = vec3(0.,       ht/300,    0.); //->green
    else if (ht < 1000)  col = vec3(ht/500-1, 1.,        0.); //->yellow
    else if (ht < 2000)  col = vec3(1.,       2.-ht/1000,0.); //->red
    else                 col = vec3(1.,       ht/2000-1 ,ht/2000-1);  //->white

    //if (ht < 0)
    //    col = vec3(0.0, 0.0, 1.0);
    //else if (ht < 2000)
    //    col = vec3(0.0, 1.0, 0.0);
    //else
    //    col - vec3(1.0, 0.0, 0.0);
    

    color = vec4(col, 1.0);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}