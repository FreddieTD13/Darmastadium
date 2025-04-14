//WIP
#include <windows.h>
#include <cmath>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <future>

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

// --- Bouncing Square Variables ---
const int SQUARE_SIZE = 100;
int squareX = 200;
int squareY = 200;
int velocityX = 10;
int velocityY = 6;
DWORD lastColorChangeTime = 0;
const DWORD COLOR_CHANGE_INTERVAL = 10;

COLORREF colors[] = {
    RGB(255, 0, 0),
    RGB(255, 165, 0),
    RGB(255, 255, 0),
    RGB(0, 255, 0),
    RGB(0, 0, 255),
    RGB(75, 0, 130),
    RGB(148, 0, 211),
    RGB(255, 0, 255)
};

int currentColorIndex = 0;
int nextColorIndex = 1;
float transitionProgress = 0.0f;

// --- Screen Distortion Variables ---
bool distortScreen = true;
DWORD lastDistortTime = 0;
const DWORD DISTORT_INTERVAL = 100;

// --- Function Declarations ---
void drawSquare(HDC hdc);
void showMessageBox();
void distortScreenFunc();
COLORREF interpolateColor(COLORREF color1, COLORREF color2, float progress);

// --- Function Definitions ---

COLORREF interpolateColor(COLORREF color1, COLORREF color2, float progress) {
    int r1 = GetRValue(color1);
    int g1 = GetGValue(color1);
    int b1 = GetBValue(color1);
    int r2 = GetRValue(color2);
    int g2 = GetGValue(color2);
    int b2 = GetBValue(color2);

    int r = static_cast<int>(r1 + (r2 - r1) * progress);
    int g = static_cast<int>(g1 + (g2 - g1) * progress);
    int b = static_cast<int>(b1 + (b2 - b1) * progress);

    return RGB(r, g, b);
}

void drawSquare(HDC hdc) {
    COLORREF currentColor = interpolateColor(colors[currentColorIndex], colors[nextColorIndex], transitionProgress);
    HBRUSH hBrush = CreateSolidBrush(currentColor);
    SelectObject(hdc, hBrush);
    Rectangle(hdc, squareX, squareY, squareX + SQUARE_SIZE, squareY + SQUARE_SIZE);
    DeleteObject(hBrush);
}

void showMessageBox() {
    MessageBoxA(NULL, "ÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆ", "ÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆ", MB_ICONERROR | MB_OK);
}

void distortScreenFunc() {
    Sleep(28000); // Wait 28 seconds before starting distortion
    HDC hdc = GetDC(0);
    if (hdc == NULL) {
        std::cerr << "Failed to get screen DC in distortScreenFunc" << std::endl;
        return;
    }

    int w = GetSystemMetrics(0), h = GetSystemMetrics(1), x;
    srand(time(0));

    while (distortScreen) {
        x = rand() % w;
        BitBlt(hdc, x, 1, 10, h, hdc, x, 0, NOTSRCCOPY);
        Sleep(100);
    }
    ReleaseDC(0, hdc);
}

int main() {
    HDC hdc = GetDC(NULL);

    if (hdc == NULL) {
        std::cerr << "Failed to get screen DC in main" << std::endl;
        return 1;
    }

    // Create a new thread for the message box
    std::thread messageBoxThread(showMessageBox);

    // Create a thread for the screen distortion, which now waits 28 seconds
    std::thread distortThread(distortScreenFunc);

    while (true) {
        DWORD currentTime = GetTickCount();

        // --- Color change for the square ---
        if (currentTime - lastColorChangeTime >= COLOR_CHANGE_INTERVAL) {
            transitionProgress += 0.1f;
            lastColorChangeTime = currentTime;
            if (transitionProgress >= 1.0f) {
                transitionProgress = 0.0f;
                currentColorIndex = (currentColorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
                nextColorIndex = (currentColorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
            }
        }

        drawSquare(hdc);

        squareX += velocityX;
        squareY += velocityY;

        if (squareX < 0 || squareX + SQUARE_SIZE > screenWidth) {
            velocityX = -velocityX;
        }
        if (squareY < 0 || squareY + SQUARE_SIZE > screenHeight) {
            velocityY = -velocityY;
        }

        Sleep(10);
        BitBlt(hdc, 0, 0, screenWidth, screenHeight, NULL, 0, 0, SRCCOPY);
    }

    ReleaseDC(NULL, hdc);
    distortScreen = false;
    distortThread.join();
    messageBoxThread.detach();
    return 0;
}
