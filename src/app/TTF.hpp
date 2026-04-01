#pragma once

namespace app {
class TTF : public utility::Singleton<TTF>
{
    friend class utility::Singleton<TTF>;

private:
    bool m_Success = false;

private:
    TTF()
    {
        if (!TTF_Init()) {
            LOGC("failed to initialize SDL_TTF");
            std::terminate();
        } else {
            m_Success = true;
        }
    }

    ~TTF()
    {
        if (m_Success) TTF_Quit();
    }
};
}
