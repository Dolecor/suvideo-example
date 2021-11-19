#include <stdio.h>
#include <suvideo.h>

#define videoName       argv[1]
#define framesDir       argv[2]
#define outVideoName    argv[3]

int main(int argc, char *argv[])
{
    if(argc != 4){
        printf("Wrong parameters\n");
        getchar();
        return -1;
    }

    // Split video to frames
    splitInit(videoName, framesDir);

    printf("Number of frames: \t%d\n",
            numFrames());

    printf("Frame rate: \t%d/%d\n",
            frameRateNum(), frameRateDenom());

    // Split video to frames
    splitAll();

    // or:
    /*while (hasNextFrame()) {
        splitFrameByFrame();
        printf("status of splitting: %d/%d\n", processedFrames(), numFrames());
    }*/

    // Do something with frames...

    // You can use the following to go through the splitted frames:
    /*  int it = 0;
        while(it != numFrames()) {
            sprintf(frameName, "%s/%s%d.png",
                    framesDir, FRAME_PTN, it);
            it++;
        }
    */

    // Make new video with [modified] frames
    unite(videoName, framesDir, outVideoName);

    // Delete frames
    deleteFrames(framesDir);

    // call this function after split and unite functions
    splitFree();

    return 0;
}