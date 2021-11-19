#include <windows.h>
#include <string.h>

#include "viduptypes.h"
#include "suvideo.h"


/**
 * Сreating "raw" video from frames
 * @param framesDir directory where frames located
 * @param videoFromFrames name of "raw" video
 * @return process handle on success, NULL if process not created
 */
HANDLE ProcessCreateRawVideo(const char *framesDir, const char *videoFromFrames)
{

    int frNum   = frameRateNum();
    int frDenom = frameRateDenom();

    char commandLine[1024] = {0};
    char CMD_makeVidFromFrames[512];

    sprintf(CMD_makeVidFromFrames,
            "-y -r %d/%d -i %s/%s%%d.png -c:v libx264 %s",
            frNum, frDenom, framesDir, FRAME_PTN, videoFromFrames);

    printf("%s", commandLine);

    sprintf(commandLine, "ffmpeg.exe %s", CMD_makeVidFromFrames);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if(!CreateProcess(NULL, commandLine,
                      NULL, NULL, FALSE, 0, NULL, NULL,
                      &si, &pi))
    {
        return NULL;
    }

    return pi.hProcess;
}


/**
 * Creating video that contains:
 * video stream from videoFromFrames,
 * audio&subtitles from srcVideoPath
 * @param srcVideoPath
 * @param videoFromFrames
 * @param outputVideoName
 * @return
 */
HANDLE ProcessCreateVideo(const char *srcVideoPath, const char *videoFromFrames,
                          const char *outputVideoName)
{
    char commandLine[1024] = {0};
    char CMD_replaceVideoStream[512];
    sprintf(CMD_replaceVideoStream,
            "-y -i %s -i %s -map 0:a? -map 1:v -c copy %s",
            srcVideoPath, videoFromFrames, outputVideoName);

    sprintf(commandLine, "ffmpeg.exe %s", CMD_replaceVideoStream);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if(!CreateProcess(NULL, commandLine,
                      NULL, NULL, FALSE, 0, NULL, NULL,
                      &si, &pi))
    {
        return NULL;
    }

    return pi.hProcess;
}


/* ------------------------------------------------------------------------ */
int unite(const char* srcVideoPath, const char *framesDir,
        const char *outputVideoName)
{
    char videoFromFrames[254];
    sprintf(videoFromFrames, "%s/videoStream.mp4", framesDir);

    HANDLE hProcRawVideo = ProcessCreateRawVideo(framesDir, videoFromFrames);
    if(hProcRawVideo == NULL){
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return -1;  // error on creating process
    }

    WaitForSingleObject(hProcRawVideo, INFINITE);

    HANDLE hProcVideo = ProcessCreateVideo(srcVideoPath, videoFromFrames,
            outputVideoName);

    if(hProcVideo == NULL){
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return -1;  // error on creating process
    }

    WaitForSingleObject(hProcVideo, INFINITE);
    if(remove(videoFromFrames) != 0) {
        printf( "remove() failed (%d).\n", GetLastError() );
        return -1;  // error on deleting temporary file
    }

    CloseHandle(hProcRawVideo);
    CloseHandle(hProcVideo);


    return 0;
}
