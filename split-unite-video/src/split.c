#include <libattopng.h>

#include "viduptypes.h"
#include "suvideo.h"



/* Utils ------------------------------------------------------------------ */

static const char *get_averror_text(const int error)
{
    static char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
}


static void save_frame_as_png(AVFrame *pFrame, size_t width, size_t height,
                       size_t frameNum, const char *outputDir)
#define RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16) | (0xff << 24))
#define R(x, y) 3*(x + y*width) + 0
#define G(x, y) 3*(x + y*width) + 1
#define B(x, y) 3*(x + y*width) + 2
{
    char szFilename[512];

    sprintf(szFilename, "%s/%s%d.png", outputDir, FRAME_PTN, (int)frameNum);

    libattopng_t* png = libattopng_new(width, height, PNG_RGB);

    uint8_t *pPixel = pFrame->data[0];
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            libattopng_set_pixel(png,
                    x, y,
                    RGB(pPixel[R(x, y)],
                        pPixel[G(x, y)],
                        pPixel[B(x, y)])
            );
        }
    }

    libattopng_save(png, szFilename);
    libattopng_destroy(png);
}

/* Utils end -------------------------------------------------------------- */



// Variables used for video processing ------------------------------------ */

static AVFormatContext     *pFormatCtx = NULL;
static AVCodec             *pCodec = NULL;
static int                 videoStream;
static AVCodecContext      *pCodecCtx = NULL;

static AVFrame             *pFrame = NULL;
static AVFrame             *pFrameRGB = NULL;
static int                 numBytes;
static uint8_t             *buffer = NULL;
static struct SwsContext   *swsCtx = NULL;

static AVPacket            packet;
static size_t              frameNum = 1;
static int                 error;

static char                *inputVideo = NULL;
static char                *outDir = NULL;


/* ------------------------------------------------------------------------ */
int splitInit(const char *inputVideoPath, const char *outputDir)
{
    if (avformat_open_input(&pFormatCtx, inputVideoPath, NULL, NULL) != 0) {
        return -1;  // cannot open input file
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        return -1;  // cannot find stream information
    }

    videoStream = av_find_best_stream(pFormatCtx,
                                      AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
    if (videoStream < 0) {
        return -1;  // no video stream
    }

    pCodec = avcodec_find_decoder(
            pFormatCtx->streams[videoStream]->codecpar->codec_id);
    if (pCodec == NULL) {
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        return -1;
    }

    if (avcodec_parameters_to_context(pCodecCtx,
            pFormatCtx->streams[videoStream]->codecpar) != 0)
    {
        return -1; // couldn't copy codec context
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) != 0) {
        return -1;
    }

    pFrame      = av_frame_alloc();
    pFrameRGB   = av_frame_alloc();

    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
            pCodecCtx->width, pCodecCtx->height, 1);
    buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize,
            buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

    swsCtx = sws_getContext(pCodecCtx->width,
                            pCodecCtx->height,
                            pCodecCtx->pix_fmt,
                            pCodecCtx->width,
                            pCodecCtx->height,
                            AV_PIX_FMT_RGB24,
                            SWS_FAST_BILINEAR,
                            NULL, NULL, NULL);

    inputVideo  = strdup(inputVideoPath);
    outDir      = strdup(outputDir);

    return 0;
}


/* ------------------------------------------------------------------------ */
int splitFree()
{
    avformat_close_input(&pFormatCtx);
    avcodec_free_context(&pCodecCtx);

    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrame);
    av_freep(&buffer);

    free(inputVideo);
    free(outDir);

    return 0;
}


/* ------------------------------------------------------------------------ */
int hasNextFrame()
{
    return av_read_frame(pFormatCtx, &packet) == 0 ? 1 : 0;
}


/* ------------------------------------------------------------------------ */
int numFrames()
{
    return pFormatCtx->streams[videoStream]->nb_frames;
}


/* ------------------------------------------------------------------------ */
int processedFrames()
{
    return frameNum;
}


/* ------------------------------------------------------------------------ */
int frameRateNum()
{
    return pFormatCtx->streams[videoStream]->avg_frame_rate.num;
}


/* ------------------------------------------------------------------------ */
int frameRateDenom()
{
    return pFormatCtx->streams[videoStream]->avg_frame_rate.den;
}


/* ------------------------------------------------------------------------ */
int splitFrameByFrame()
{
    if (packet.stream_index == videoStream) {
        // Decode video frame (avcodec_send_packet + avcodec_receive_frame)
        error = avcodec_send_packet(pCodecCtx, &packet);
        if (error < 0) {
            fprintf(stderr,
                    "Could not send packet for decoding (error '%s')\n",
                    get_averror_text(error));
            return -1;
        }
        error = avcodec_receive_frame(pCodecCtx, pFrame);
        if (error == AVERROR(EAGAIN)) {
            fprintf(stderr, "No output available yet \n");
            return 0;   // need to call the function again
        }
        else if (error < 0) {
            fprintf(stderr,
                    "Error during decoding (error '%s')\n",
                    get_averror_text(error));
            return -1;
        }

        // Convert the image from its native format to RGB
        sws_scale(swsCtx, (uint8_t const * const *)pFrame->data,
                  pFrame->linesize, 0, pCodecCtx->height,
                  pFrameRGB->data, pFrameRGB->linesize);

        save_frame_as_png(pFrameRGB,
                          (size_t)pCodecCtx->width,
                          (size_t)pCodecCtx->height,
                          frameNum,
                          outDir);
        ++frameNum;
    }
    av_packet_unref(&packet);

    return 0;
}


/* ------------------------------------------------------------------------ */
int splitAll()
{
    while (av_read_frame(pFormatCtx, &packet) == 0) {
        splitFrameByFrame();
    }

    return 0;
}