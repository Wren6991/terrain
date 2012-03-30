#ifndef VERT_H_INCLUDED
#define VERT_H_INCLUDED

struct vert
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;             //32 byte aligned :D
};

#endif // VERT_H_INCLUDED
