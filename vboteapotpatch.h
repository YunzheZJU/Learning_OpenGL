#ifndef VBOTEAPOTPATCH_H
#define VBOTEAPOTPATCH_H

#include "global.h"
#include "drawable.h"

class VBOTeapotPatch : public Drawable
{
private:
    unsigned int vaoHandle;

    void generatePatches(float * v);
    void buildPatchReflect(int patchNum,
                           float *v, int &index,
                           bool reflectX, bool reflectY);
    void buildPatch(vec3 patch[][4],
                    float *v, int &index, mat3 reflect);
    void getPatch( int patchNum, vec3 patch[][4], bool reverseV );

public:
    VBOTeapotPatch();

    void render() const;
};

#endif // VBOTEAPOTPATCH_H
