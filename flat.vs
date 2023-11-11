#version 330 core

layout (location = 0) in float z;

uniform float lon;
uniform float lat;
uniform mat4 view;
uniform mat4 projection;

vec2 mercator(float londeg, float latdeg) {
    float lat = radians(latdeg);
    float lon = radians(londeg);
    
    return vec2(lon,  log(abs(1/cos(lat) + tan(lat))));
}

out float ht;


void main() {
    vec2 coords = mercator(lon, lat);

    vec2 span;
    float lonc, latc;

    if (lon < 0)
        lonc = lon - 1;
    else 
        lonc = lon + 1;

    if (lat < 0)
        latc = lat - 1;
    else 
        latc = lat + 1;

    span = (mercator(lonc, latc) - coords)/1200;
    
    float x = coords.x + mod(gl_VertexID, 1201)*span.x;
    float y = coords.y + floor(gl_VertexID/1201.0)*span.y;

    ht = z;
    gl_Position = projection * view * vec4(x, y, 0.0, 1.0);
}

