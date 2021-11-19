/**
 * @file Header file suvideo.h
 *
 * This file contains definitions of functions for splitting
 * video into frames and creating video from frames.
 */

#ifndef SUVIDEO_H
#define SUVIDEO_H

#include <stdio.h>
#include <stdlib.h>

#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libavutil\imgutils.h"



#define FRAME_PTN "su_frame"


/**
 * Initialize variables for split* function
 * @note should be called before some of split* function
 * @param inputVideoPath video which should be splitted to frames
 * @param outputDir directory where frames will be located
 * @return zero on success, nonzero on error
 */
int splitInit(const char *inputVideoPath, const char *outputDir);

/**
 * Free resources
 * @note should be called after some of split* function
 */
int splitFree();

/**
 * Checks if there is a next frame in the video stream
 * @attention you must call splitInit() before using this function.
 * @return 1 if there is a frame, 0 otherwise
 */
int hasNextFrame();

/**
 * @attention you must call splitInit() before using this function.
 * @return number of frames in video
 */
int numFrames();

/**
 * @attention you must call splitInit() before using this function.
 * @return number of splitted frames
 */
int processedFrames();

int frameRateNum();

int frameRateDenom();

/**
 * Save one frame from input video
 * with name {FRAME_PTN}{frNum}.png,
 * where frNum is current decoded frame.
 * @note frNum is a static (internal) variable and incremented
 *          by function call splitFrameByFrame()
 * @attention you must call splitInit() before using this function.
 * @example
 * @code
 *  splitInit(videoname, framesdir);
 *  while (hasNextFrame()){
 *      splitFrameByFrame();
 *  }
 *  splitFree();
 * @endcode
 * @return zero on success, nonzero on error
 */
int splitFrameByFrame();

/**
 * Split video (by path inputVideoPath)
 * into frames and save them by path outputDir
 * with names FRAME_PTN%d.png
 * @attention you must call splitInit() before using this function.
 * @example
 * @code
 *  splitInit(videoname, framesdir);
 *  splitAll();
 *  splitFree();
 * @endcode
 * @return zero on success, nonzero on error
 */
int splitAll();



/**
 * Unite frames in framesDir ({framesDir}/{FRAME_PTN}%d.png)
 * into video (by path framesDir/outputVideoName)
 * adding audio&subtitles from source video (by path {srcVideoPath})
 * @attention you must call splitInit() before using this function.
 * @param srcVideoPath source video from which
 *              audio&subtitles will be taken
 * @param framesDir directory with frames to make video stream
 * @param outputVideoName name of output video file
 * @return  zero on success, nonzero on error (to be determined)
 * @todo specification of errors
 */
int unite(const char* srcVideoPath, const char *framesDir,
        const char *outputVideoName);



/**
 * Delete frames produced by one of split* function
 * @param framesDir directory where frames located
 */
int deleteFrames(const char *framesDir);

#endif //SUVIDEO_H