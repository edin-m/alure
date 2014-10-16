#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#include <stdint.h>
inline void Sleep(uint32_t ms)
{
    struct timespec ts, rem;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms & 1000) * 1000000;
    while(nanosleep(&ts, &rem) == -1 && errno == EINTR)
        ts = rem;
}
#endif

#include <iostream>

#include "alure2.h"

int main(int argc, char *argv[])
{
    alure::DeviceManager *devMgr = alure::DeviceManager::get();

    alure::Device *dev = devMgr->openPlayback();
    std::cout<< "Opened \""<<dev->getName(alure::PlaybackDevType_Basic)<<"\"" <<std::endl;

    alure::Context *ctx = dev->createContext();
    alure::Context::MakeCurrent(ctx);

    for(int i = 1;i < argc;i++)
    {
        alure::Decoder *decoder = ctx->createDecoder(argv[i]);
        alure::Source *source = ctx->getSource();
        source->play(decoder, 32768, 4);
        std::cout<< "Playing "<<argv[i]<<" ("<<decoder->getFrequency()<<"hz)" <<std::endl;

        while(source->isPlaying())
        {
            Sleep(25);
            ctx->update();
        }

        ctx->finalize(source);
        source = 0;
        delete decoder;
        decoder = 0;
    }

    alure::Context::MakeCurrent(0);
    ctx->destroy();
    ctx = 0;
    dev->close();
    dev = 0;

    return 0;
}