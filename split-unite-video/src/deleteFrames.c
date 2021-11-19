#include "viduptypes.h"
#include "suvideo.h"



int deleteFrames(const char *framesDir)
{
    int frameNum = 1;
    char frameName[512];

    do {
        sprintf(frameName,
                "%s/%s%d.png",
                framesDir, FRAME_PTN, frameNum);
        frameNum++;
    } while (remove(frameName) == 0);

    return 0;
}
