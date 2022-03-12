#ifndef APP_HPP
#define APP_HPP

#include "analyser.hpp"
#include <thread>

// #define WHITE { 1.0f, 1.0f, 1.0f, 1.0f }
// #define BLACK { 0.0f, 0.0f, 0.0f, 1.0f }
// #define RED { 1.0f, 0.0f, 0.0f, 1.0f }
// #define GREEN { 0.0f, 1.0f, 0.0f, 1.0f }
// #define BLUE { 0.0f, 0.0f, 1.0f, 1.0f }
// #define YELLOW { 0.0f, 1.0f, 1.0f, 1.0f }
// #define MAGENTA { 1.0f, 0.0f, 1.0f, 1.0f }

namespace nano
{

class App
{
private:
    static inline raylib::Window window;
    static inline raylib::Image currImg;
    static inline std::string currImgPath;
    static inline raylib::Texture* currTexture = nullptr;
    static inline raylib::Texture* maskTexture = nullptr;
    static inline raylib::Texture* tubeMaskTexture = nullptr;
    static inline Analyser analyser;
    static inline raylib::Vector2 cameraPosition;
    static inline float cameraZoom = 1.0f;
    static inline std::thread worker;


    // flags
    static inline bool workerIsDone = false;
    static inline bool calculating = false;
    static inline bool maskVisible = true;
    static inline bool tubeMaskVisible = true;
    //

    static int init(int windowWidth, int windowHeight, const char* windowName);
    static void setDroppedImg();
    static void setWindowSize(raylib::Vector2 size);

    static void setMaskTexture();
    static void setTubeMaskTexture();

    static void processControls();
    static void startExtremumAnalysis();
    static void startManualAnalysis(float threshold);
    static void cancelAnalysis();

    friend class UI;

    App() = delete;
    App(const App& other) = delete;
public:
    static int run(int windowWidth, int widnowHeight, const char* windowName);
    static void free();
};

} // namespace nano

#endif // APP_HPP