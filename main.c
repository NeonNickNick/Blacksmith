#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <process.h>
#include "constant.h"
#include "skill.h"
#include "mystring.h"
#include "realrandom.h"
#include "text.h"
#include "connect.h"
#include "math.h"
typedef enum {
    MAIN_MENU,
    HELP_MENU,
    SCROLL_MENU,
    END_MENU,
    ONLINE_MENU,
    CREATE_MENU,
    JOIN_MENU
} MenuState;

MenuState currentMenuState = MAIN_MENU;
int receivenumber = -1;
int sentnumber = -1;
bool is_playing = false;

bool is_begin = false;
HANDLE hChildStd_IN_Wr, hChildStd_OUT_Rd;
char receive[BUFFER_SIZE];
bool if_robotplus = false;

unsigned __stdcall ReadOutputThread(void *param) {
    DWORD bytesRead;
    while (1) {
        DWORD dwAvailable;
        if (PeekNamedPipe(hChildStd_OUT_Rd, NULL, 0, NULL, &dwAvailable, NULL) && dwAvailable > 0) {
            if (ReadFile(hChildStd_OUT_Rd, receive, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
                receive[bytesRead] = '\0';
                char *a = strstr(receive, connection);
                char *b = strstr(receive, connected);
                if (a || b) {
                    if (!prequit) {
                        initgamer();
                        is_begin = true;
                        currentMenuState = SCROLL_MENU;
                        is_playing = true;
                    }
                }

                if(!if_plus) {
                    chartoint(receive, &receivenumber);
                    if(receivenumber != -1) {
                        if_plus = !if_plus;
                    }
                    if (receivenumber == SCROLL_BUTTON_COUNT + 10) {
                        robot.healthpoints = 0;
                        break;
                    }
                }else {
                    chartoint(receive, &robotplus);
                    if(robotplus != -1) {
                        if_robotplus = true;
                        if_plus = !if_plus;
                    }
                }
            }
        }
        Sleep(10);
    }
    return 0;
}



bool is_checking = false;
int is_clicking = -1;



typedef struct {
    SDL_Rect rect;
    const char *text;
} Button;



typedef struct {
    SDL_Rect rect;
    const char *text;
    bool visible;
    bool isLocked;
} TextPopup;

// 弹窗参数
TextPopup popup = {{(WINDOW_WIDTH - 600) / 2, (WINDOW_HEIGHT - 300) / 2, 600, 300}, "", false};
// 渲染
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
// 字体
TTF_Font *font = NULL;
TTF_Font *titlefont = NULL;
TTF_Font *littlefont = NULL;
// 滚动偏移量
int scrollOffset_play = 0;
int scrollOffset_help = 0;
// 当前界面状态

// 渲染按钮函数
SDL_Color textColor = {0, 0, 0}; // 黑色文字
SDL_Color bgColor = {255, 255, 255}; // 白色背景
SDL_Color forbidColor = {128, 128, 128}; // 禁用按钮
SDL_Color grayColor = {169, 169, 169}; // 灰色背景
SDL_Color culColor = {199, 199, 199}; // 计算
SDL_Color redColor = {225, 225, 0};
SDL_Color greenColor = {0, 255, 0};
// 游戏部分开始
int is_calculate = 0;
int is_done = 1;
//将所有资源从-1开始进行编号：-1生命0铁1魔2金铁3空间4时间...
//将所有特殊效果从-1开始编号：-1无效果0破甲(3倍)1禁言2反伤
void RenderMultilineText(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    SDL_Color color,
    int startX,
    int startY,
    int lineSpace
) {
    // 复制原始文本（strtok会修改字符串）
    char *textCopy = strdup(text);
    // 获取实际行间距
    int actualLineSpace = (lineSpace > 0) ? lineSpace : TTF_FontLineSkip(font);
    int currentY = startY;

    // 分割每一行
    char *line = strtok(textCopy, "\n");
    while (line != NULL) {
        // 跳过空行
        if (strlen(line) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }
        // 渲染单行文本
        SDL_Surface *surface = TTF_RenderText_Solid(font, line, color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        // 计算位置并渲染
        SDL_Rect destRect = {
            .x = startX,
            .y = currentY,
            .w = surface->w,
            .h = surface->h
        };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        // 更新下一行位置
        currentY += actualLineSpace;
        // 释放资源
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        line = strtok(NULL, "\n");
    }
    free(textCopy); // 释放复制的字符串
}

// 游戏部分结束
void renderButton(const Button *button, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color) {
    // 渲染按钮背景
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &button->rect);

    // 渲染按钮文字
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, button->text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
        button->rect.x + (button->rect.w - textSurface->w) / 2,
        button->rect.y + (button->rect.h - textSurface->h) / 2,
        textSurface->w, textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// 初始化SDL
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        // 确保包括SDL_INIT_AUDIO
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Blacksmith V1.0", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // 初始化SDL_mixer
    if (Mix_Init(MIX_INIT_MP3) == 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("SDL_mixer could not initialize audio! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    font = TTF_OpenFont("Merienda-Regular.ttf", 24);
    titlefont = TTF_OpenFont("Merienda-Bold.ttf", 48);
    littlefont = TTF_OpenFont("Merienda-Bold.ttf", 16);
    return true;
}

// 清理SDL资源
void closeSDL() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    if (Mix_PlayingMusic()) {

        Mix_HaltMusic();
    }
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

// 背景音乐
void playBackgroundMusic() {

    if (Mix_PlayMusic(Mix_LoadMUS("D:/Code/test/cmake-build-debug/BGM.mp3"), -1) == -1) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

// 联机大厅
void renderMultipleMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 标题
    Button titleButton = {{(WINDOW_WIDTH - 500) / 2, 150, 500, 100}, "Multiple"};
    renderButton(&titleButton, renderer, titlefont, bgColor);
    // 功能按钮
    Button buttons[] = {
        {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 350, BUTTON_WIDTH, BUTTON_HEIGHT}, "Create"},
        {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420, BUTTON_WIDTH, BUTTON_HEIGHT}, "Join"}
    };
    for (int i = 0; i < 2; i++) {
        renderButton(&buttons[i], renderer, font, bgColor);
    }
    Button backButton = {{20, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back"};
    renderButton(&backButton, renderer, font, bgColor);
}

void rendercreateMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 标题
    Button titleButton = {{(WINDOW_WIDTH - 900) / 2, 250, 900, 100}, "Tell you friend the roomnumber..."};
    renderButton(&titleButton, renderer, titlefont, bgColor);

    if (strlen(porter) && !prequit) {
        char room[100] = {"Roomnumber: "};
        for (int i = 0; i < 5; ++i) {
            room[i + 12] = porter[i];
        }
        Button roomButton = {{(WINDOW_WIDTH - 550) / 2, 450, 550, 100}, room};
        renderButton(&roomButton, renderer, titlefont, bgColor);
    }

    Button backButton = {{20, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back"};
    renderButton(&backButton, renderer, font, bgColor);
}

void renderjoinMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 标题
    Button titleButton = {{(WINDOW_WIDTH - 1000) / 2, 250, 1000, 100}, "Enter the roomnumber and press ENTER"};
    renderButton(&titleButton, renderer, titlefont, bgColor);

    if (1) {
        char room[100] = {"Roomnumber: "};
        for (int i = 0; i < portsite; ++i) {
            room[i + 12] = port[i];
        }
        Button roomButton = {{(WINDOW_WIDTH - 550) / 2, 450, 550, 100}, room};
        renderButton(&roomButton, renderer, titlefont, bgColor);
    }

    Button backButton = {{20, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back"};
    renderButton(&backButton, renderer, font, bgColor);
}

// 主菜单渲染
void renderMainMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 标题
    Button titleButton = {{(WINDOW_WIDTH - 500) / 2, 150, 500, 100}, "Blacksmith V1.0"};
    renderButton(&titleButton, renderer, titlefont, bgColor);
    // 功能按钮
    if (is_playing == false) {
        if (is_done) {
            Button buttons[] = {
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 350, BUTTON_WIDTH, BUTTON_HEIGHT}, "Single"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420, BUTTON_WIDTH, BUTTON_HEIGHT}, "Multiple"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 490, BUTTON_WIDTH, BUTTON_HEIGHT}, "Help"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 560, BUTTON_WIDTH, BUTTON_HEIGHT}, "Exit"}
            };
            for (int i = 0; i < 4; i++) {
                renderButton(&buttons[i], renderer, font, bgColor);
            }
        } else {
            Button buttons[] = {
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 350, BUTTON_WIDTH, BUTTON_HEIGHT}, "Wait"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420, BUTTON_WIDTH, BUTTON_HEIGHT}, "Wait"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 490, BUTTON_WIDTH, BUTTON_HEIGHT}, "Help"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 560, BUTTON_WIDTH, BUTTON_HEIGHT}, "Exit"}
            };
            for (int i = 0; i < 4; i++) {
                renderButton(&buttons[i], renderer, font, bgColor);
            }
        }
    } else {
        if (is_begin) {
            Button buttons[] = {
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 350, BUTTON_WIDTH, BUTTON_HEIGHT}, "single"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420, BUTTON_WIDTH, BUTTON_HEIGHT}, "Continue"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 490, BUTTON_WIDTH, BUTTON_HEIGHT}, "Help"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 560, BUTTON_WIDTH, BUTTON_HEIGHT}, "Exit"}
            };
            for (int i = 0; i < 4; i++) {
                if (i == 0) {
                    renderButton(&buttons[i], renderer, font, grayColor);
                    continue;
                }
                renderButton(&buttons[i], renderer, font, bgColor);
            }
        } else {
            Button buttons[] = {
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 350, BUTTON_WIDTH, BUTTON_HEIGHT}, "Continue"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420, BUTTON_WIDTH, BUTTON_HEIGHT}, "Multiple"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 490, BUTTON_WIDTH, BUTTON_HEIGHT}, "Help"},
                {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 560, BUTTON_WIDTH, BUTTON_HEIGHT}, "Exit"}
            };
            for (int i = 0; i < 4; i++) {
                if (i == 1) {
                    renderButton(&buttons[i], renderer, font, grayColor);
                    continue;
                }
                renderButton(&buttons[i], renderer, font, bgColor);
            }
        }
    }
}

void renderEndMenu() {
    is_playing = 0;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // 标题
    Button titleButton_1 = {{(WINDOW_WIDTH - 500) / 2, 150, 500, 100}, "DRAW !"};
    Button titleButton_2 = {{(WINDOW_WIDTH - 500) / 2, 150, 500, 100}, "YOU LOSE !"};
    Button titleButton_3 = {{(WINDOW_WIDTH - 500) / 2, 150, 500, 100}, "YOU WIN !"};
    if (player.healthpoints <= 0 && robot.healthpoints <= 0) {
        renderButton(&titleButton_1, renderer, titlefont, bgColor);
    } else if (player.healthpoints <= 0) {
        renderButton(&titleButton_2, renderer, titlefont, bgColor);
    } else if (robot.healthpoints <= 0) {
        renderButton(&titleButton_3, renderer, titlefont, bgColor);
    }
    // 功能按钮
    Button buttons[] = {
        {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 350, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back"},
        {{(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420, BUTTON_WIDTH, BUTTON_HEIGHT}, "Exit"}
    };
    for (int i = 0; i < 2; i++) {
        renderButton(&buttons[i], renderer, font, bgColor);
    }
}

// 滑动条
void renderScrollBar(int totalHeight, int windowHeight, SDL_Renderer *renderer, int scrollOffset) {
    int scrollBarHeight = windowHeight * windowHeight / totalHeight;
    if (scrollBarHeight > windowHeight) scrollBarHeight = windowHeight;

    int maxOffset = totalHeight - windowHeight;
    if (maxOffset <= 0) return;

    int scrollBarY = (scrollOffset * (windowHeight - scrollBarHeight)) / maxOffset;

    SDL_SetRenderDrawColor(renderer, 169, 200, 169, 255);
    SDL_Rect scrollBarRect = {WINDOW_WIDTH - 20, 10, 10, windowHeight};
    SDL_RenderFillRect(renderer, &scrollBarRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect scrollBarThumb = {WINDOW_WIDTH - 20, 10 + scrollBarY, 10, scrollBarHeight};
    SDL_RenderFillRect(renderer, &scrollBarThumb);
}

// 帮助界面渲染（完整实现）
void renderHelpMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // 标题
    Button titleButton = {{(WINDOW_WIDTH - 200) / 2, 40, 200, 50}, "Help Menu"};
    renderButton(&titleButton, renderer, font, bgColor);

    // 返回按钮
    Button backButton = {{20, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back"};
    renderButton(&backButton, renderer, font, bgColor);

    // 可滚动区域的边框
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 白色
    SDL_Rect scrollArea = {20, 100, WINDOW_WIDTH - 40, WINDOW_HEIGHT - 150}; // 边框
    SDL_RenderDrawRect(renderer, &scrollArea);

    SDL_Color textColor = {255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, helpText, textColor, WINDOW_WIDTH - 40);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // 文本区域
    SDL_Rect textRect = {
        scrollArea.x + 10, scrollArea.y - scrollOffset_help, textSurface->w - 10, textSurface->h
    }; // 根据滚动位置调整文本位置

    // 设置渲染裁剪区域：仅在白色边框内渲染文字
    SDL_Rect clipRect = {scrollArea.x, scrollArea.y, scrollArea.w, scrollArea.h};
    SDL_RenderSetClipRect(renderer, &clipRect);

    // 渲染文本
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // 取消裁剪
    SDL_RenderSetClipRect(renderer, NULL);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // 绘制滚动条
    renderScrollBar(HELPHEIGHT, WINDOW_HEIGHT, renderer, scrollOffset_help);
    Button line_0 = {{0, 0, WINDOW_WIDTH, 10}, " "};
    renderButton(&line_0, renderer, font, grayColor);
    Button line_6 = {{0, 790, WINDOW_WIDTH, 10}, " "};
    renderButton(&line_6, renderer, font, grayColor);
    Button line_5 = {{0, 5, 10, 800}, " "};
    renderButton(&line_5, renderer, font, grayColor);
    Button line_7 = {{1190, 0, 10, 800}, " "};
    renderButton(&line_7, renderer, font, grayColor);
}

// 帮助弹窗
void renderPopup(const TextPopup *popup, SDL_Renderer *renderer, TTF_Font *font) {
    if (!popup->visible) return; // 如果弹窗不可见，直接返回，不渲染

    // 渲染弹出框背景为白色
    SDL_Color bgColor = {255, 255, 255, 255}; // 白色背景
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &popup->rect);

    // 渲染文本
    SDL_Color textColor = {0, 0, 0}; // 黑色文本
    SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, popup->text, textColor, popup->rect.w - 20); // 自动换行
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // 计算文本的宽度和高度
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // 设置文本绘制区域
    SDL_Rect textRect = {popup->rect.x + 10, popup->rect.y + 10, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // 绘制关闭按钮
    Button closeButton = {{popup->rect.x + popup->rect.w - 40, popup->rect.y + 10, 30, 30}, "X"};
    renderButton(&closeButton, renderer, font, bgColor);

    // 清理
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void DrawArrow(SDL_Renderer *renderer, float x, float y, float size, float angleDegrees) {
    // 转为弧度
    float angle = angleDegrees * PI / 180.0f;

    // 箭头尾部到箭头头部的向量
    float dx = cos(angle) * size;
    float dy = sin(angle) * size;

    // 箭头头部的位置
    float x2 = x + dx;
    float y2 = y + dy;

    // 绘制箭头主体线
    SDL_RenderDrawLine(renderer, (int) x, (int) y, (int) x2, (int) y2);

    // 箭头两侧小角度，用于箭头头部（30度）
    float arrowHeadAngle1 = angle + PI / 6;
    float arrowHeadAngle2 = angle - PI / 6;

    float arrowSize = 200 * 0.3f; // 箭头头部长度为箭头大小的 30%

    // 箭头左边线
    float x3 = x2 - cos(arrowHeadAngle1) * arrowSize;
    float y3 = y2 - sin(arrowHeadAngle1) * arrowSize;
    SDL_RenderDrawLine(renderer, (int) x2, (int) y2, (int) x3, (int) y3);

    // 箭头右边线
    float x4 = x2 - cos(arrowHeadAngle2) * arrowSize;
    float y4 = y2 - sin(arrowHeadAngle2) * arrowSize;
    SDL_RenderDrawLine(renderer, (int) x2, (int) y2, (int) x4, (int) y4);
}

void playernotice(int *a, int *b, int *c) {
    if (playercommand == 0 || playercommand == 24) {
        *b = 55;
    }
    if ((playercommand >= 1 && playercommand <= 3) || playercommand == 10 || playercommand == 12 || playercommand == 14
        || playercommand == 18 || playercommand == 26 || playercommand == 28) {
        *b = -90;
    }
    if (playercommand == 22) {
        *b = -37;
    }
    if (playercommand == 8 || playercommand == 29) {
        *b = 42;
    }
    if (playercommand == 9 || playercommand == 30) {
        *b = 33;
    }
    if (playercommand == 15) {
        *b = 27;
    }
    if (playercommand == 17 || playercommand == 23 || playercommand == 27) {
        *a = 65;
        *b = 6;
    }
    *c = abs(*a / sin(*b * PI / 180));
}

void robotnotice(int *a, int *b, int *c) {
    if (robotcommand == 0 || robotcommand == 24) {
        *b = -144;
    }
    if ((robotcommand >= 1 && robotcommand <= 3) || robotcommand == 10 || robotcommand == 12 || robotcommand == 14 ||
        robotcommand == 18 || robotcommand == 26 || robotcommand == 28) {
        *b = 153;
    }
    if (robotcommand == 22) {
        *b = 122;
    }
    if (robotcommand == 8 || robotcommand == 29) {
        *b = -129;
    }
    if (robotcommand == 9 || robotcommand == 30) {
        *b = -116;
    }
    if (robotcommand == 15) {
        *b = -90;
    }
    if (robotcommand == 17 || robotcommand == 23 || robotcommand == 27) {
        *a = 190;
        *b = -58;
    }
    *c = abs(*a / sin(*b * PI / 180));
}

void renderScrollButtonsMenu() {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    // 返回按钮
    Button backButton = {{20, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back"};
    renderButton(&backButton, renderer, font, bgColor);

    //结束按钮
    Button restartButton = {{30 + BUTTON_WIDTH, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Restart"};
    renderButton(&restartButton, renderer, font, bgColor);

    //帮助按钮
    Button helpButton = {{40 + 2 * BUTTON_WIDTH, 20, BUTTON_WIDTH, BUTTON_HEIGHT}, "Help"};
    renderButton(&helpButton, renderer, font, bgColor);

    //引导按钮
    Button guideButton = {{50 + 3 * BUTTON_WIDTH, 20, BUTTON_WIDTH / 2, BUTTON_HEIGHT / 2}, "Guidance?"};
    renderButton(&guideButton, renderer, littlefont, bgColor);
    Button confirmButton = {{50 + 13 * BUTTON_WIDTH / 4 - 10, 25 + BUTTON_HEIGHT / 2, 20, 20}, " "};
    if (if_guide) {
        renderButton(&confirmButton, renderer, littlefont, greenColor);
    } else {
        renderButton(&confirmButton, renderer, littlefont, grayColor);
    }
    //回合
    char roundtext[50];
    sprintf(roundtext, "Round: %d", rounds);
    Button roundButton = {{4 * BUTTON_WIDTH - 30, 10, BUTTON_WIDTH, BUTTON_HEIGHT + 20}, roundtext};
    renderButton(&roundButton, renderer, font, bgColor);

    //分割线
    Button line_1 = {{10, 30 + BUTTON_HEIGHT, WINDOW_WIDTH - BUTTON_WIDTH - 35, 5}, " "};
    renderButton(&line_1, renderer, font, grayColor);

    Button line_2 = {{WINDOW_WIDTH - BUTTON_WIDTH - 30, 0, 10, 800}, " "};
    renderButton(&line_2, renderer, font, grayColor);

    Button line_3 = {{WINDOW_WIDTH - BUTTON_WIDTH - 235, 0, 5, 680 + BUTTON_HEIGHT}, " "};
    renderButton(&line_3, renderer, font, grayColor);

    Button line_4 = {{10, 680 + BUTTON_HEIGHT, WINDOW_WIDTH - BUTTON_WIDTH - 35, 5}, " "};
    renderButton(&line_4, renderer, font, grayColor);


    // 滚动按钮列表
    for (int i = 0; i < SCROLL_BUTTON_COUNT; i++) {
        SDL_Rect btnRect = {
            WINDOW_WIDTH - BUTTON_WIDTH - 20,
            10 + (i * 60) - scrollOffset_play,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };

        if (btnRect.y + btnRect.h < 0 || btnRect.y > WINDOW_HEIGHT) {
            continue;
        }

        Button scrollButton = {btnRect, name_skill[i]};
        if ((blacksmith_skill[i].funcskill)(&player, 0, 0)) {
            blacksmith_skill[i].is_valid = 1;
        } else {
            blacksmith_skill[i].is_valid = 0;
        }
        if (is_calculate) {
            renderButton(&scrollButton, renderer, font, culColor);
        } else {
            if (blacksmith_skill[i].is_valid) {
                renderButton(&scrollButton, renderer, font, bgColor);
            } else {
                renderButton(&scrollButton, renderer, font, forbidColor);
            }
        }
    }


    // 绘制滚动条
    renderScrollBar(SCROLL_BUTTON_COUNT * 60, WINDOW_HEIGHT, renderer, scrollOffset_play);
    Button line_6 = {{0, 0, WINDOW_WIDTH, 10}, " "};
    renderButton(&line_6, renderer, font, grayColor);
    Button line_7 = {{0, 790, WINDOW_WIDTH, 10}, " "};
    renderButton(&line_7, renderer, font, grayColor);
    Button line_8 = {{0, 5, 10, 800}, " "};
    renderButton(&line_8, renderer, font, grayColor);
    Button line_9 = {{1190, 0, 10, 800}, " "};
    renderButton(&line_9, renderer, font, grayColor);

    //行动按钮
    if (playercommand != -1) {
        char pl[5] = {"+"};
        char mo[100];
        if(playercommand == 4 || playercommand == 5 || playercommand == 18 || playercommand == 19 || playercommand == 20) {
            char num[15];
            char co1[20];
            intochar(num, playerplus);
            comb(co1, pl, num);
            comb(mo, name_skill[playercommand], co1);
        }else {
            char nu[2] = {"\0"};
            comb(mo, name_skill[playercommand], nu);
        }
        Button playerButton = {{10, 360 + BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT}, mo};
        renderButton(&playerButton, renderer, font, bgColor);
        int angle = 90;
        int r = 250;
        int len = 250;
        playernotice(&r, &angle, &len);
        if (if_guide) {
            DrawArrow(renderer, 10 + BUTTON_WIDTH / 2, 355 + BUTTON_HEIGHT, len, angle);
        }
    }
    if (robotcommand != -1) {
        char pl[5] = {"+"};
        char mo[100];
        if(robotcommand == 4 || robotcommand == 5 || robotcommand == 18 || robotcommand == 19 || robotcommand == 20) {
            char num[15];
            char co1[20];
            intochar(num, robotplus);
            comb(co1, pl, num);
            comb(mo, name_skill[robotcommand], co1);
        }else {
            char nu[2] = {"\0"};
            comb(mo, name_skill[robotcommand], nu);
        }
        Button robotButton = {
            {WINDOW_WIDTH - 2 * BUTTON_WIDTH - 235, 355, BUTTON_WIDTH, BUTTON_HEIGHT}, mo
        };
        renderButton(&robotButton, renderer, font, bgColor);
        int angle = -153;
        int r = 250;
        int len = 250;
        robotnotice(&r, &angle, &len);
        if (if_guide) {
            DrawArrow(renderer, WINDOW_WIDTH - 1.5 * BUTTON_WIDTH - 235, 359 + BUTTON_HEIGHT, len, angle);
        }
    }
    Button line_5 = {{10, 355 + BUTTON_HEIGHT, WINDOW_WIDTH - BUTTON_WIDTH - 35, 5}, " "};
    renderButton(&line_5, renderer, font, grayColor);
    // 如果弹窗已显示，渲染弹出框
    if (popup.visible) {
        renderPopup(&popup, renderer, font);
    }
}

void renderGamerData(SDL_Renderer *renderer, gamer_basic player, gamer_basic robot, MenuState menustate) {
    if (menustate != SCROLL_MENU) {
        return;
    }
    char playerText[512];
    char robotText[512];
    char playerclassText[512] = {" \0"};
    char robotclassText[512] = {" \0"};

    // 格式化文本
    snprintf(playerText, sizeof(playerText),
             "Player HP: %d/%d | Iron: %d&%d | Space: %d | Time: %d | Armor: %d",
             player.healthpoints, player.maxhealthpoints, player.res_iron, player.res_goldiron,
             player.res_space, player.res_time, player.shield_common_static);

    snprintf(robotText, sizeof(robotText),
             "Robot HP: %d/%d | Iron: %d&%d | Space: %d | Time: %d | Armor: %d",
             robot.healthpoints, robot.maxhealthpoints,
             robot.res_iron, robot.res_goldiron, robot.res_space, robot.res_time, robot.shield_common_static);

    SDL_Color whiteColor = {255, 255, 255};
    if (player.serialnumber_class == 0) {
        if (player.Wand->is_alchemy == 0) {
            snprintf(playerclassText, sizeof(playerclassText), "Class: Wand\nMagic: %d", player.res_magic);
        } else {
            snprintf(playerclassText, sizeof(playerclassText), "Class: Wand\nAlchemy Book\nMagic: %d",
                     player.res_magic);
        }
        RenderMultilineText(renderer, font, playerclassText, whiteColor, WINDOW_WIDTH - BUTTON_WIDTH - 215, 420, 0);
    }
    if (robot.serialnumber_class == 0) {
        if (robot.Wand->is_alchemy == 0) {
            snprintf(robotclassText, sizeof(robotclassText), "Class: Wand\nMagic: %d", robot.res_magic);
        } else {
            snprintf(robotclassText, sizeof(robotclassText), "Class: Wand\nAlchemy Book\nMagic: %d", robot.res_magic);
        }
        RenderMultilineText(renderer, font, robotclassText, whiteColor, WINDOW_WIDTH - BUTTON_WIDTH - 215, 95, 0);
    }
    if (player.serialnumber_class == 1) {
        snprintf(playerclassText, sizeof(playerclassText), "Class: Driver\nTime Shield: %d", player.Driver->timeshield);
        RenderMultilineText(renderer, font, playerclassText, whiteColor, WINDOW_WIDTH - BUTTON_WIDTH - 215, 420, 0);
    }
    if (robot.serialnumber_class == 1) {
        snprintf(robotclassText, sizeof(robotclassText), "Class: Driver\nTime shield: %d", robot.Driver->timeshield);
        RenderMultilineText(renderer, font, robotclassText, whiteColor, WINDOW_WIDTH - BUTTON_WIDTH - 215, 95, 0);
    }

    // 创建文本表面
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, playerText, whiteColor); // 白色
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {20, 680, textSurface->w, textSurface->h}; // 文本位置
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // 渲染机器人数据
    SDL_Surface *robotSurface = TTF_RenderText_Solid(font, robotText, whiteColor);
    SDL_Texture *robotTexture = SDL_CreateTextureFromSurface(renderer, robotSurface);
    SDL_Rect robotRect = {20, 95, robotSurface->w, robotSurface->h};
    SDL_RenderCopy(renderer, robotTexture, NULL, &robotRect);
    // 清理
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(robotTexture);
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(robotSurface);
}

void updatemovement_effect(gamer_basic *gamer, movement **move) {
    movement *temp = (movement *) malloc(sizeof(movement));
    temp->next = *move;
    movement *current = temp;
    while (current->next != NULL) {
        if (current->next->round == 0) {
            if (current->next->effect == 1) {
                gamer->if_muted = true;
            }
            movement *remove = current->next;
            current->next = remove->next;
            free(remove);
            continue;
        }
        current = current->next;
    }
    *move = temp->next;
    free(temp);
}

void updatemovement_def(int *change, movement **move) {
    movement *temp_head = (movement *) malloc(sizeof(movement));
    temp_head->next = *move;
    movement *current = temp_head;
    while (current->next != NULL) {
        current->next->round -= 1;
        if (current->next->round == -1) {
            movement *to_remove = current->next;
            *change += to_remove->power;
            current->next = to_remove->next;
            free(to_remove);
        } else {
            current = current->next;
        }
    }
    *move = temp_head->next;
    free(temp_head);
    temp_head = NULL;
}

int myminus(int a, int b) {
    int c = a - b;
    return c > 0 ? c : 0;
}

void updatemovement_offset(movement **a, movement **b) {
    movement *temp_head_a = (movement *) malloc(sizeof(movement));
    movement *temp_head_b = (movement *) malloc(sizeof(movement));
    temp_head_a->next = *a;
    temp_head_b->next = *b;
    movement *current_a = temp_head_a;
    movement *current_b = temp_head_b;
    while (current_a->next != NULL && current_b->next != NULL) {
        current_a->next->round -= 1;
        current_b->next->round -= 1;
        if (current_a->next->round > -1 || current_b->next->round > -1) {
            if (current_a->next->round > -1) {
                current_a = current_a->next;
            }
            if (current_b->next->round > -1) {
                current_b = current_b->next;
            }
            continue;
        }
        int p = current_a->next->power;
        int q = current_b->next->power;
        current_a->next->power = myminus(p, q);
        current_b->next->power = myminus(q, p);

        movement *to_remove_a = current_a->next;
        movement *to_remove_b = current_b->next;
        if (current_a->next->power == 0) {
            current_a->next = to_remove_a->next;
            free(to_remove_a);
        }
        if (current_b->next->power == 0) {
            current_b->next = to_remove_b->next;
            free(to_remove_b);
        }
    }
    while (current_a->next != NULL) {
        current_a->next->round -= 1;
        current_a = current_a->next;
    }
    while (current_b->next != NULL) {
        current_b->next->round -= 1;
        current_b = current_b->next;
    }

    *a = temp_head_a->next;
    *b = temp_head_b->next;
    free(temp_head_a);
    free(temp_head_b);
    temp_head_a = NULL;
    temp_head_b = NULL;
}

void updatemovement_harm_physic(int *plaharmeff, movement **plamove, int robDEF, int robRDEF, int robThorn,
                                gamer_basic *rob) {
    movement *temp_head = (movement *) malloc(sizeof(movement));
    temp_head->next = *plamove;
    movement *current = temp_head;
    while (current->next != NULL) {
        if (current->next->round <= -1) {
            movement *to_remove = current->next;
            bool if_damage_common = false;
            int effect = to_remove->effect;
            int origin = to_remove->power;
            origin = myminus(origin, robRDEF);
            if (origin != 0 && robDEF + robThorn != 0) {
                if_damage_common = true;
            }
            int damage;
            if (effect == 0) {
                origin *= 3;
            }
            damage = origin > robThorn ? robThorn : origin;
            if (damage % 2 != 0) {
                damage += 2;
            }
            damage /= 2;
            linkmovement(damage, 0, -1, &(rob->attack_magic));
            origin = myminus(origin, robThorn + robDEF);
            if (effect == 0) {
                if (origin % 3 != 0) {
                    origin += 3;
                }
                origin /= 3;
            }
            int real = rob->shield_real_static;
            rob->shield_real_static = real > origin ? real - origin : 0;
            origin = myminus(origin, real);
            if (effect == 0) {
                origin *= 3;
            }
            if (origin != 0) {
                if_damage_common = true;
            }
            int common = rob->shield_common_static;
            rob->shield_common_static = common > origin ? common - origin : 0;
            origin = myminus(origin, common);
            if (effect == 0) {
                if (origin % 3 != 0) {
                    origin += 3;
                }
                origin /= 3;
            }
            *plaharmeff += origin;
            if (effect == 0 && if_damage_common) {
                rob->if_pierced = true;
            }
            current->next = to_remove->next;
            free(to_remove);
        } else {
            current = current->next;
        }
    }
    *plamove = temp_head->next;
    free(temp_head);
    temp_head = NULL;
}

void updatemovement_harm_magic(int *plaharmeff, movement **plamove, int robDEF, int robRDEF, int robThorn,
                               gamer_basic *rob) {
    movement *temp_head = (movement *) malloc(sizeof(movement));
    temp_head->next = *plamove;
    movement *current = temp_head;
    while (current->next != NULL) {
        if (current->next->round <= -1) {
            movement *to_remove = current->next;


            int origin = to_remove->power;

            origin = myminus(origin, robRDEF + robThorn + robDEF);

            int real = rob->shield_real_static;
            rob->shield_real_static = real > origin ? real - origin : 0;
            origin = myminus(origin, real);

            int common = rob->shield_common_static;
            rob->shield_common_static = common > origin ? common - origin : 0;
            origin = myminus(origin, common);

            *plaharmeff += origin;
            current->next = to_remove->next;
            free(to_remove);
        } else {
            current = current->next;
        }
    }
    *plamove = temp_head->next;
    free(temp_head);
    temp_head = NULL;
}

void updatemovement_res(gamer_basic *gamer, movement **move) {
    movement *temp_head = (movement *) malloc(sizeof(movement));
    temp_head->next = *move;
    movement *current = temp_head;
    while (current->next != NULL) {
        current->next->effect -= 1;
        if (current->next->effect == -2) {
            movement *to_remove = current->next;

            if (to_remove->round == -1 && !gamer->if_pierced) {
                gamer->healthpoints += to_remove->power;
                gamer->healthpoints = gamer->healthpoints > gamer->maxhealthpoints
                                          ? gamer->maxhealthpoints
                                          : gamer->healthpoints;
            }
            if (to_remove->round == 0 && !gamer->if_pierced) {
                gamer->res_iron += to_remove->power;
            }
            if (to_remove->round == 1 && !gamer->if_pierced) {
                gamer->res_magic += to_remove->power;
            }
            if (to_remove->round == 2 && !gamer->if_pierced) {
                gamer->res_goldiron += to_remove->power;
            }
            if (to_remove->round == 3 && ((!gamer->if_muted) || gamer->muteimmune)) {
                gamer->res_space += to_remove->power;
            }
            if (to_remove->round == 4 && ((!gamer->if_muted) || gamer->muteimmune)) {
                gamer->res_time += to_remove->power;
            }

            current->next = to_remove->next;
            free(to_remove);
        } else {
            current = current->next;
        }
    }
    *move = temp_head->next;
    free(temp_head);
    temp_head = NULL;
}

int findsum(movement **move) {
    movement *temp = *move;
    int re = 0;
    while (temp != NULL) {
        if (temp->round == 0) {
            re += temp->power;
        }
        temp = temp->next;
    }
    return re;
}

int findmax(movement **move) {
    movement *temp = *move;
    int re = 0;
    while (temp != NULL) {
        if (temp->round == 0) {
            re = re > temp->power ? re : temp->power;
        }
        temp = temp->next;
    }
    return re;
}

void findeff(int *a, movement **move) {
    movement *temp = *move;
    while (temp != NULL) {
        if (temp->round == 0) {
            a[temp->effect] = 1;
        }
        temp = temp->next;
    }
}

int decide(int known) {
    int index = 0;
    int eff[10] = {0};
    // 选职业
    if (rounds >= 6 && robot.serialnumber_class == -1) {
        if (player.serialnumber_class != -1 && robot.res_usableiron >= 6) {
            return 25;
        }
        if (player.serialnumber_class == -1 && robot.res_usableiron >= 2) {
            return 16;
        }
    }
    if (rounds == 2) {
        if (myrandom() <= 0.5) {
            return 16;
        }
    }
    if (rounds > 3 && robot.res_usableiron >= 6 && robot.serialnumber_class == -1) {
        if (myrandom() <= 0.289) {
            return 25;
        }
    }

    // 分类讨论
    findeff(eff, &player.effects);
    int playerdef = findsum(&(player.shield_common)) + findsum(&(player.shield_real)) + findsum(&(player.shield_thorn));
    playerdef += player.shield_real_static;
    playerdef += player.shield_common_static;
    int robotdef = findsum(&(robot.shield_common)) + findsum(&(robot.shield_real)) + findsum(&(robot.shield_thorn));
    robotdef += robot.shield_real_static;
    robotdef += robot.shield_common_static;
    int playermaxmagic = findsum(&(player.attack_magic));
    int playermaxphysic = findsum(&(player.attack_physic));
    int playerratk = findsum(&(player.attack_real));
    if (robot.serialnumber_class == -1) {
        if (robot.res_usableiron >= 6 && myrandom() <= 0.8) {
            if (player.serialnumber_class == 0 && eff[1] && !(robot.muteimmune)) {
                ;
            } else {
                return 8;
            }
        }

        if (1) {
            if (robot.res_space != 0 && playerdef + player.healthpoints <= 8) {
                return 10;
            } else if (robot.res_usableiron >= 5 && playerdef + player.healthpoints <= 5) {
                return 3;
            } else if (robot.res_usableiron >= 3 && playerdef + player.healthpoints <= 3) {
                return 2;
            } else if (robot.res_usableiron >= 1 && playerdef + player.healthpoints <= 1) {
                return 1;
            }
        }
        return 0;
    }
    if (robot.serialnumber_class == 0) {
        if (known == 8 || known == 9 || known == 29 || known == 30) {
            return 22;
        }
        if (robot.healthpoints >= 8) {
            return 21;
        }
        if (robot.healthpoints > 1 && robot.maxhealthpoints >= 6 && playermaxmagic <= 8 && playermaxphysic <= 8 &&
            playermaxmagic >= 2 && playermaxphysic >= 2 && robot.res_magic == 0) {
            return 21;
        }
    }
    return index;
}

void transmitcopy(movement **a, movement **b) {
    movement *temp = *a;
    while (temp != NULL) {
        if (temp->round == 0) {
            linkmovement(temp->power, 0, temp->effect, b);
            temp->power = 0;
            temp->effect = -1;
        }
        temp = temp->next;
    }
}

void transmitAtoB(gamer_basic *A, gamer_basic *B) {
    transmitcopy(&(A->attack_magic), &(B->attack_magic));
    transmitcopy(&(A->attack_physic), &(B->attack_physic));
    transmitcopy(&(A->attack_real), &(B->attack_real));
    transmitcopy(&(A->effects), &(B->effects));
}

void delay(movement **move) {
    movement *temp = *move;
    while (temp != NULL) {
        if (temp->round == 0) {
            temp->round = 3;
        }
        temp = temp->next;
    }
}

void delaygamer(gamer_basic *a) {
    delay(&(a->attack_magic));
    delay(&(a->attack_physic));
    delay(&(a->attack_real));
    delay(&(a->effects));
}

void *settlement(void *arg) {
    init_rand();
    int index_player = *(int *) arg;
    is_done = 0;
    is_calculate = 1;
    int index_robot = 0;

    blacksmith_skill[index_player].funcskill(&player, playerplus, 1);
    index_robot = decide(index_player);
    blacksmith_skill[index_robot].funcskill(&robot, 0, 1);
    playercommand = index_player;
    robotcommand = index_robot;
    if (player.trans == true && robot.trans == true) {
        player.trans = false;
        robot.trans = false;
    }
    if (player.trans == true) {
        player.trans = false;
        transmitAtoB(&robot, &player);
    }
    if (robot.trans == true) {
        robot.trans = false;
        transmitAtoB(&player, &robot);
    }

    if (player.delay) {
        --player.delay;
        delaygamer(&robot);
    }
    if (robot.delay) {
        --robot.delay;
        delaygamer(&player);
    }


    updatemovement_effect(&robot, &player.effects);
    updatemovement_effect(&player, &robot.effects);

    int used = player.res_iron + player.res_goldiron - player.res_usableiron;
    int u = player.res_goldiron;
    player.res_goldiron = myminus(u, used);
    used = myminus(used, u);
    player.res_iron -= used;

    used = robot.res_iron + robot.res_goldiron - robot.res_usableiron;
    u = robot.res_goldiron;
    robot.res_goldiron = myminus(u, used);
    used = myminus(used, u);
    robot.res_iron -= used;


    int player_DEF = 0;
    int robot_DEF = 0;
    int player_RDEF = 0;
    int robot_RDEF = 0;
    int player_Thorn = 0;
    int robot_Thorn = 0;
    updatemovement_def(&player_DEF, &(player.shield_common));
    updatemovement_def(&robot_DEF, &(robot.shield_common));
    updatemovement_def(&player_RDEF, &(player.shield_real));
    updatemovement_def(&robot_RDEF, &(robot.shield_real));
    updatemovement_def(&player_Thorn, &(player.shield_thorn));
    updatemovement_def(&robot_Thorn, &(robot.shield_thorn));
    if (player.serialnumber_class == 1) {
        player_RDEF += player.Driver->timeshield;
    }
    if (robot.serialnumber_class == 1) {
        robot_RDEF += robot.Driver->timeshield;
    }

    int player_RATK = 0;
    int robot_RATK = 0;
    updatemovement_def(&player_RATK, &(player.attack_real));
    updatemovement_def(&robot_RDEF, &(robot.attack_real));


    updatemovement_offset(&(player.attack_physic), &(robot.attack_physic));
    updatemovement_offset(&(player.attack_magic), &(robot.attack_magic));
    updatemovement_offset(&(player.attack_physic), &(robot.attack_magic));
    updatemovement_offset(&(player.attack_magic), &(robot.attack_physic));


    int player_ATK = player_RATK;
    int robot_ATK = robot_RATK;

    updatemovement_harm_physic(&player_ATK, &(player.attack_physic), robot_DEF, robot_RDEF, robot_Thorn, &robot);
    updatemovement_harm_physic(&robot_ATK, &(robot.attack_physic), player_DEF, player_RDEF, player_Thorn, &player);
    updatemovement_harm_magic(&player_ATK, &(player.attack_magic), robot_DEF, robot_RDEF, robot_Thorn, &robot);
    updatemovement_harm_magic(&robot_ATK, &(robot.attack_magic), player_DEF, player_RDEF, player_Thorn, &player);

    updatemovement_res(&player, &(player.get_resource));
    updatemovement_res(&robot, &(robot.get_resource));

    if (!player.invincible) {
        player.healthpoints -= robot_ATK;
    }
    if (!player.invincible) {
        robot.healthpoints -= player_ATK;
    }

    player.res_usableiron = player.res_iron + player.res_goldiron;
    robot.res_usableiron = robot.res_iron + robot.res_goldiron;

    player.if_muted = false;
    player.if_pierced = false;
    robot.if_muted = false;
    robot.if_pierced = false;
    if (player.invincible) {
        --player.invincible;
    }
    if (robot.invincible) {
        --robot.invincible;
    }
    if (player.muteimmune) {
        --player.muteimmune;
    }
    if (robot.muteimmune) {
        --robot.muteimmune;
    }
    is_calculate = 0;
    ++rounds;
    if (!is_playing) {
        initgamer();
    }
    is_done = 1;
    return NULL;
}

bool is_online = false;
char sent[BUFFER_SIZE];

void *create_server(void *arg) {
    HANDLE hChildStd_IN_Rd, hChildStd_OUT_Wr;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0);
    SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

    HANDLE hNullInput = CreateFile("NUL", GENERIC_READ, FILE_SHARE_READ, &saAttr, OPEN_EXISTING, 0, NULL);
    if (hNullInput == INVALID_HANDLE_VALUE) {
        printf("创建NUL句柄失败\n");
        return NULL;
    }

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = hNullInput; // SSH进程不接收输入
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    char sshCmd[] = "cmd.exe /c ssh -T -o LogLevel=INFO -R 0:localhost:1234 serveo.net";

    if (!CreateProcess(NULL, sshCmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo)) {
        printf("SSH进程创建失败\n");
        CloseHandle(hNullInput);
        return NULL;
    }
    CloseHandle(hNullInput); // 关闭NUL句柄

    // 读取 SSH 输出内容
    char sshOutput[BUFFER_SIZE + 1] = {0};
    DWORD sshBytesRead = 0;
    int port = 0;
    while (ReadFile(hChildStd_OUT_Rd, sshOutput, BUFFER_SIZE, &sshBytesRead, NULL) && sshBytesRead > 0) {
        sshOutput[sshBytesRead] = '\0';
        // 提取端口号示例：查找 serveo.net: 之后的端口
        char *pos = strstr(sshOutput, "serveo.net:");
        if (pos) {
            sscanf(pos + strlen("serveo.net:"), "%d", &port);
            break;
        }
    } /*else {
        printf("Fail\n");
    }*/
    // 创建 ncat 进程
    PROCESS_INFORMATION piProcInfoNcat;
    STARTUPINFO siStartInfoNcat;
    ZeroMemory(&piProcInfoNcat, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfoNcat, sizeof(STARTUPINFO));
    siStartInfoNcat.cb = sizeof(STARTUPINFO);
    siStartInfoNcat.hStdError = hChildStd_OUT_Wr;
    siStartInfoNcat.hStdOutput = hChildStd_OUT_Wr;
    siStartInfoNcat.hStdInput = hChildStd_IN_Rd;
    siStartInfoNcat.dwFlags |= STARTF_USESTDHANDLES;

    char cmd[] = "cmd.exe /c ncat -lvp 1234";
    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfoNcat, &piProcInfoNcat)) {
        printf("CreateProcess fail\n");
        return NULL;
    }

    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    // Create a separate thread to read output
    uintptr_t threadHandle = _beginthreadex(NULL, 0, ReadOutputThread, NULL, 0, NULL);
    if (!threadHandle) {
        printf("Failed to create read thread\n");
        return NULL;
    }
    DWORD bytesWritten;
    intochar(porter, port);
    scrollOffset_play = 0;
    while (1) {
        int copyplus = 0;
        while (sentnumber == -1) {
            if (prequit) {
                porter[0] = '\0';
                break;
            }
            copyplus = playerplus;
            Sleep(10);
        }
        if (prequit) {
            break;
        }
        playercommand = sentnumber;
        intochar(sent, sentnumber);
        char cplus[100];
        intochar(cplus, copyplus);
        WriteFile(hChildStd_IN_Wr, sent, strlen(sent), &bytesWritten, NULL);
        Sleep(500);
        WriteFile(hChildStd_IN_Wr, cplus, strlen(cplus), &bytesWritten, NULL);
        if (sentnumber == SCROLL_BUTTON_COUNT + 10) {
            break;
        }
        int extra = 0;
        is_done = 0;
        is_calculate = 1;

        while (receivenumber == -1 || !if_robotplus) {
            if (prequit) {
                porter[0] = '\0';
                break;
            }
            Sleep(10);
        }
        if (prequit) {
            break;
        }
        robotcommand = receivenumber;
        if (receivenumber == SCROLL_BUTTON_COUNT + 10) {
            initgamer();
            is_calculate = 0;
            is_done = 1;
            is_playing = 0;
            is_online = 0;
            is_begin = 0;
            sentnumber = -1;
            receivenumber = -1;
            break;
        }
        blacksmith_skill[sentnumber].funcskill(&player, copyplus, 1);
        blacksmith_skill[receivenumber].funcskill(&robot, robotplus, 1);

        if (player.trans == true && robot.trans == true) {
            player.trans = false;
            robot.trans = false;
        }
        if (player.trans == true) {
            player.trans = false;
            transmitAtoB(&robot, &player);
        }
        if (robot.trans == true) {
            robot.trans = false;
            transmitAtoB(&player, &robot);
        }

        if (player.delay) {
            --player.delay;
            delaygamer(&robot);
        }
        if (robot.delay) {
            --robot.delay;
            delaygamer(&player);
        }


        updatemovement_effect(&robot, &player.effects);
        updatemovement_effect(&player, &robot.effects);

        int used = player.res_iron + player.res_goldiron - player.res_usableiron;
        int u = player.res_goldiron;
        player.res_goldiron = myminus(u, used);
        used = myminus(used, u);
        player.res_iron -= used;

        used = robot.res_iron + robot.res_goldiron - robot.res_usableiron;
        u = robot.res_goldiron;
        robot.res_goldiron = myminus(u, used);
        used = myminus(used, u);
        robot.res_iron -= used;


        int player_DEF = 0;
        int robot_DEF = 0;
        int player_RDEF = 0;
        int robot_RDEF = 0;
        int player_Thorn = 0;
        int robot_Thorn = 0;
        updatemovement_def(&player_DEF, &(player.shield_common));
        updatemovement_def(&robot_DEF, &(robot.shield_common));
        updatemovement_def(&player_RDEF, &(player.shield_real));
        updatemovement_def(&robot_RDEF, &(robot.shield_real));
        updatemovement_def(&player_Thorn, &(player.shield_thorn));
        updatemovement_def(&robot_Thorn, &(robot.shield_thorn));
        if (player.serialnumber_class == 1) {
            player_RDEF += player.Driver->timeshield;
        }
        if (robot.serialnumber_class == 1) {
            robot_RDEF += robot.Driver->timeshield;
        }

        int player_RATK = 0;
        int robot_RATK = 0;
        updatemovement_def(&player_RATK, &(player.attack_real));
        updatemovement_def(&robot_RDEF, &(robot.attack_real));


        updatemovement_offset(&(player.attack_physic), &(robot.attack_physic));
        updatemovement_offset(&(player.attack_magic), &(robot.attack_magic));
        updatemovement_offset(&(player.attack_physic), &(robot.attack_magic));
        updatemovement_offset(&(player.attack_magic), &(robot.attack_physic));


        int player_ATK = player_RATK;
        int robot_ATK = robot_RATK;

        updatemovement_harm_physic(&player_ATK, &(player.attack_physic), robot_DEF, robot_RDEF, robot_Thorn, &robot);
        updatemovement_harm_physic(&robot_ATK, &(robot.attack_physic), player_DEF, player_RDEF, player_Thorn, &player);
        updatemovement_harm_magic(&player_ATK, &(player.attack_magic), robot_DEF, robot_RDEF, robot_Thorn, &robot);
        updatemovement_harm_magic(&robot_ATK, &(robot.attack_magic), player_DEF, player_RDEF, player_Thorn, &player);

        updatemovement_res(&player, &(player.get_resource));
        updatemovement_res(&robot, &(robot.get_resource));

        if (!player.invincible) {
            player.healthpoints -= robot_ATK;
        }
        if (!player.invincible) {
            robot.healthpoints -= player_ATK;
        }

        player.res_usableiron = player.res_iron + player.res_goldiron;
        robot.res_usableiron = robot.res_iron + robot.res_goldiron;

        player.if_muted = false;
        player.if_pierced = false;
        robot.if_muted = false;
        robot.if_pierced = false;
        if (player.invincible) {
            --player.invincible;
        }
        if (robot.invincible) {
            --robot.invincible;
        }
        if (player.muteimmune) {
            --player.muteimmune;
        }
        if (robot.muteimmune) {
            --robot.muteimmune;
        }

        is_calculate = 0;
        ++rounds;
        if (!is_playing) {
            initgamer();
        }
        is_done = 1;

        sentnumber = -1;
        receivenumber = -1;
    }

    CloseHandle(hChildStd_IN_Wr);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(piProcInfoNcat.hProcess);
    CloseHandle(piProcInfoNcat.hThread);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    is_calculate = 0;
    ++rounds;
    if (!is_playing) {
        initgamer();
    }
    is_done = 1;

    sentnumber = -1;
    receivenumber = -1;
    prequit = false;
    return NULL;
}

void *create_client(void *arg) {
    HANDLE hChildStd_IN_Rd, hChildStd_OUT_Wr;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0);
    SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    while (!is_begin) {
        // printf("port: ");
        // scanf("%s", port);
        portsite = 0;
        port[0] = '\0';
        while (portsite < 6 && !prequit) {
            Sleep(10);
        }
        if (prequit) {
            portsite = 0;
            port[0] = '\0';
            break;
        }
        port[5] = '\0';
        char cmd[100];
        sprintf(cmd, "cmd.exe /c ncat -v serveo.net %s", port);

        if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo)) {
            printf("CreateProcess fail\n");
            return NULL;
        }
        // Create a separate thread to read output
        uintptr_t threadHandle = _beginthreadex(NULL, 0, ReadOutputThread, NULL, 0, NULL);
        if (!threadHandle) {
            printf("Failed to create read thread\n");
            return NULL;
        }
        Sleep(2000);
    }
    portsite = 0;
    port[0] = '\0';
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);
    DWORD bytesWritten;
    scrollOffset_play = 0;
    while (1) {
        int copyplus = 0;
        while (sentnumber == -1) {
            if (prequit) {
                break;
            }
            copyplus = playerplus;
            Sleep(10);
        }
        if (prequit) {
            break;
        }
        playercommand = sentnumber;
        intochar(sent, sentnumber);
        char cplus[100];
        intochar(cplus, copyplus);
        WriteFile(hChildStd_IN_Wr, sent, strlen(sent), &bytesWritten, NULL);
        Sleep(500);
        WriteFile(hChildStd_IN_Wr, cplus, strlen(cplus), &bytesWritten, NULL);
        if (sentnumber == SCROLL_BUTTON_COUNT + 10) {
            break;
        }
        int extra = 0;
        is_done = 0;
        is_calculate = 1;
        while (receivenumber == -1 || !if_robotplus) {
            if (prequit) {
                porter[0] = '\0';
                break;
            }
            Sleep(10);
        }
        if (prequit) {
            break;
        }
        robotcommand = receivenumber;
        if (receivenumber == SCROLL_BUTTON_COUNT + 10) {
            initgamer();
            is_calculate = 0;
            is_done = 1;
            is_playing = 0;
            is_online = 0;
            is_begin = 0;
            sentnumber = -1;
            receivenumber = -1;
            break;
        }
        blacksmith_skill[sentnumber].funcskill(&player, copyplus, 1);
        blacksmith_skill[receivenumber].funcskill(&robot, robotplus, 1);

        if (player.trans == true && robot.trans == true) {
            player.trans = false;
            robot.trans = false;
        }
        if (player.trans == true) {
            player.trans = false;
            transmitAtoB(&robot, &player);
        }
        if (robot.trans == true) {
            robot.trans = false;
            transmitAtoB(&player, &robot);
        }

        if (player.delay) {
            --player.delay;
            delaygamer(&robot);
        }
        if (robot.delay) {
            --robot.delay;
            delaygamer(&player);
        }


        updatemovement_effect(&robot, &player.effects);
        updatemovement_effect(&player, &robot.effects);

        int used = player.res_iron + player.res_goldiron - player.res_usableiron;
        int u = player.res_goldiron;
        player.res_goldiron = myminus(u, used);
        used = myminus(used, u);
        player.res_iron -= used;

        used = robot.res_iron + robot.res_goldiron - robot.res_usableiron;
        u = robot.res_goldiron;
        robot.res_goldiron = myminus(u, used);
        used = myminus(used, u);
        robot.res_iron -= used;


        int player_DEF = 0;
        int robot_DEF = 0;
        int player_RDEF = 0;
        int robot_RDEF = 0;
        int player_Thorn = 0;
        int robot_Thorn = 0;
        updatemovement_def(&player_DEF, &(player.shield_common));
        updatemovement_def(&robot_DEF, &(robot.shield_common));
        updatemovement_def(&player_RDEF, &(player.shield_real));
        updatemovement_def(&robot_RDEF, &(robot.shield_real));
        updatemovement_def(&player_Thorn, &(player.shield_thorn));
        updatemovement_def(&robot_Thorn, &(robot.shield_thorn));
        if (player.serialnumber_class == 1) {
            player_RDEF += player.Driver->timeshield;
        }
        if (robot.serialnumber_class == 1) {
            robot_RDEF += robot.Driver->timeshield;
        }

        int player_RATK = 0;
        int robot_RATK = 0;
        updatemovement_def(&player_RATK, &(player.attack_real));
        updatemovement_def(&robot_RDEF, &(robot.attack_real));


        updatemovement_offset(&(player.attack_physic), &(robot.attack_physic));
        updatemovement_offset(&(player.attack_magic), &(robot.attack_magic));
        updatemovement_offset(&(player.attack_physic), &(robot.attack_magic));
        updatemovement_offset(&(player.attack_magic), &(robot.attack_physic));


        int player_ATK = player_RATK;
        int robot_ATK = robot_RATK;

        updatemovement_harm_physic(&player_ATK, &(player.attack_physic), robot_DEF, robot_RDEF, robot_Thorn, &robot);
        updatemovement_harm_physic(&robot_ATK, &(robot.attack_physic), player_DEF, player_RDEF, player_Thorn, &player);
        updatemovement_harm_magic(&player_ATK, &(player.attack_magic), robot_DEF, robot_RDEF, robot_Thorn, &robot);
        updatemovement_harm_magic(&robot_ATK, &(robot.attack_magic), player_DEF, player_RDEF, player_Thorn, &player);

        updatemovement_res(&player, &(player.get_resource));
        updatemovement_res(&robot, &(robot.get_resource));

        if (!player.invincible) {
            player.healthpoints -= robot_ATK;
        }
        if (!player.invincible) {
            robot.healthpoints -= player_ATK;
        }

        player.res_usableiron = player.res_iron + player.res_goldiron;
        robot.res_usableiron = robot.res_iron + robot.res_goldiron;

        player.if_muted = false;
        player.if_pierced = false;
        robot.if_muted = false;
        robot.if_pierced = false;
        if (player.invincible) {
            --player.invincible;
        }
        if (robot.invincible) {
            --robot.invincible;
        }
        if (player.muteimmune) {
            --player.muteimmune;
        }
        if (robot.muteimmune) {
            --robot.muteimmune;
        }
        is_calculate = 0;
        ++rounds;
        if (!is_playing) {
            initgamer();
        }
        is_done = 1;

        sentnumber = -1;
        receivenumber = -1;
    }

    CloseHandle(hChildStd_IN_Wr);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    prequit = false;
    is_calculate = 0;
    ++rounds;
    if (!is_playing) {
        initgamer();
    }
    is_done = 1;

    sentnumber = -1;
    receivenumber = -1;
    return NULL;
}

// 主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int index = 0;
    pthread_t settle;
    pthread_t server;
    pthread_t client;
    initSDL();
    bool quit = false;
    SDL_Event e;
    int dragStartY = -1; // 记录拖动开始的Y位置
    bool isDragging = false; // 是否正在拖动滚动条
    playBackgroundMusic();
    //声明技能组
    declareskill();
    //初始化玩家
    initgamer();
    // 游戏循环 开多线程
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if ((player.healthpoints <= 0 || robot.healthpoints <= 0) && is_playing == true) {
                is_playing = false;
                is_checking = false;
                is_clicking = -1;
                currentMenuState = END_MENU;
                break;
            }
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                if (currentMenuState == JOIN_MENU) {
                    if (mouseX >= 20 && mouseX <= 20 + BUTTON_WIDTH &&
                        mouseY >= 20 && mouseY <= 20 + BUTTON_HEIGHT) {
                        popup.visible = false;
                        currentMenuState = ONLINE_MENU;
                        if (!is_begin) {
                            prequit = true;
                        }
                    }
                } else if (currentMenuState == CREATE_MENU || currentMenuState == JOIN_MENU) {
                    if (mouseX >= 20 && mouseX <= 20 + BUTTON_WIDTH &&
                        mouseY >= 20 && mouseY <= 20 + BUTTON_HEIGHT) {
                        popup.visible = false;
                        currentMenuState = ONLINE_MENU;
                        if (!is_begin) {
                            prequit = true;
                        }
                    }
                } else if (currentMenuState == END_MENU) {
                    if (mouseX >= (WINDOW_WIDTH - BUTTON_WIDTH) / 2 &&
                        mouseX <= (WINDOW_WIDTH + BUTTON_WIDTH) / 2) {
                        if (mouseY >= 350 && mouseY <= 400) {
                            currentMenuState = MAIN_MENU; // 进入滚动界面
                            scrollOffset_play = 0; // 重置滚动位置
                            is_playing = 0;
                            is_begin = 0;
                            is_online = 0;
                            initgamer();
                        } else if (mouseY >= 420 && mouseY <= 470) {
                            quit = true; // 退出程序
                        }
                    }
                } else if (currentMenuState == MAIN_MENU) {
                    // 主菜单按钮判断
                    if (mouseX >= (WINDOW_WIDTH - BUTTON_WIDTH) / 2 &&
                        mouseX <= (WINDOW_WIDTH + BUTTON_WIDTH) / 2) {
                        if (mouseY >= 350 && mouseY <= 400 && (is_done || is_playing) && !is_begin) {
                            currentMenuState = SCROLL_MENU; // 进入滚动界面
                            is_playing = true;
                            scrollOffset_play = 0;
                        } else if (mouseY >= 420 && mouseY <= 470 && (is_done || is_begin)) {
                            if (is_begin) {
                                currentMenuState = SCROLL_MENU;
                                break;
                            }
                            currentMenuState = ONLINE_MENU;
                        } else if (mouseY >= 490 && mouseY <= 540) {
                            currentMenuState = HELP_MENU; // 进入帮助界面
                        } else if (mouseY >= 560 && mouseY <= 610) {
                            quit = true; // 退出程序
                        }
                    }
                } else {
                    // 返回按钮判断（所有子界面共用）
                    if (mouseX >= 20 && mouseX <= 20 + BUTTON_WIDTH &&
                        mouseY >= 20 && mouseY <= 20 + BUTTON_HEIGHT) {
                        popup.visible = false;
                        if (is_checking) {
                            currentMenuState = SCROLL_MENU;
                            is_checking = false;
                        } else {
                            currentMenuState = MAIN_MENU; // 返回主菜单
                        }
                    }
                    // 如果弹出框已显示，点击左键关闭弹出框
                    if (popup.visible && mouseX >= popup.rect.x + popup.rect.w - 40 && mouseX <= popup.rect.x + popup.
                        rect.w &&
                        mouseY >= popup.rect.y + 10 && mouseY <= popup.rect.y + 40) {
                        popup.visible = false;
                    }

                    // **修改部分开始**：限制滚动区域的点击
                    if (currentMenuState == ONLINE_MENU) {
                        if (mouseX >= (WINDOW_WIDTH - BUTTON_WIDTH) / 2 &&
                            mouseX <= (WINDOW_WIDTH + BUTTON_WIDTH) / 2) {
                            if (mouseY >= 350 && mouseY <= 400 && (is_done || is_playing)) {
                                prequit = false;
                                currentMenuState = CREATE_MENU; // 进入滚动界面
                                is_online = true;
                                initgamer();
                                pthread_create(&server, NULL, create_server, NULL);
                            } else if (mouseY >= 420 && mouseY <= 470 && is_done && !is_playing) {
                                prequit = false;
                                currentMenuState = JOIN_MENU;
                                is_online = true;
                                initgamer();
                                pthread_create(&client, NULL, create_client, NULL);
                            }
                        }
                    } else if (currentMenuState == SCROLL_MENU) {
                        for (int i = 0; i < SCROLL_BUTTON_COUNT; i++) {
                            Button button = {
                                {
                                    WINDOW_WIDTH - BUTTON_WIDTH - 20,
                                    10 + (i * 60) - scrollOffset_play,
                                    BUTTON_WIDTH, BUTTON_HEIGHT
                                },
                                "Button"
                            };
                            if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &button.rect) &&
                                blacksmith_skill[i].is_valid && (!is_calculate)) {
                                popup.visible = false;
                                playerplus = 0;
                                playercommand = i;
                            }
                        }
                        if (mouseX >= 40 + 2 * BUTTON_WIDTH && mouseX <= 40 + 3 * BUTTON_WIDTH && mouseY >= 20 && mouseY
                            <= 20 + BUTTON_HEIGHT) {
                            popup.visible = false;
                            is_checking = true;
                            currentMenuState = HELP_MENU;
                        }
                        if (mouseX >= 30 + BUTTON_WIDTH && mouseX <= 30 + 2 * BUTTON_WIDTH && mouseY >= 20 && mouseY <=
                            20 + BUTTON_HEIGHT) {
                            popup.visible = false;
                            sentnumber = SCROLL_BUTTON_COUNT + 10;
                            is_playing = false;
                            is_begin = false;
                            is_online = false;
                            scrollOffset_play = 0;
                            initgamer();
                            prequit = true;
                            currentMenuState = MAIN_MENU;
                        }
                        // 检测右侧滚动条区域点击
                        if (mouseX >= WINDOW_WIDTH - 20 && mouseX <= WINDOW_WIDTH - 10 && mouseY >= 50 && mouseY <=
                            WINDOW_HEIGHT) {
                            isDragging = true;
                            dragStartY = mouseY;
                        }
                        if (mouseX >= 50 + 13 * BUTTON_WIDTH / 4 - 10 && mouseX <= 50 + 13 * BUTTON_WIDTH / 4 + 10 &&
                            mouseY >= 25 + BUTTON_HEIGHT / 2 && mouseY <= 25 + BUTTON_HEIGHT / 2 + 20) {
                            if_guide = !if_guide;
                        }
                    }
     

                    // 检测返回按钮
                    if (currentMenuState == HELP_MENU) {
                        if (mouseX >= 20 && mouseX <= 20 + BUTTON_WIDTH && mouseY >= 20 && mouseY <= 20 +
                            BUTTON_HEIGHT) {
                            popup.visible = false;
                            currentMenuState = MAIN_MENU;
                        }
                        if (mouseX >= WINDOW_WIDTH - 20 && mouseX <= WINDOW_WIDTH - 10 && mouseY >= 50 && mouseY <=
                            WINDOW_HEIGHT) {
                            isDragging = true;
                            dragStartY = mouseY;
                        }
                    }
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                int mouseY = e.motion.y;
                if (isDragging && dragStartY != -1) {
                    int diff = mouseY - dragStartY;
                    int dragSpeed = 2.7; // 拖动灵敏度
                    if (currentMenuState == SCROLL_MENU) {
                        scrollOffset_play += diff * dragSpeed * (SCROLL_BUTTON_COUNT * MAX - WINDOW_HEIGHT) / (
                            WINDOW_HEIGHT);
                        // 更新滚动位置
                        dragStartY = mouseY;
                        // 限制拖动范围
                        scrollOffset_play = scrollOffset_play < 0 ? 0 : scrollOffset_play;
                        int maxOffset = SCROLL_BUTTON_COUNT * MAX - WINDOW_HEIGHT - 32;
                        scrollOffset_play = scrollOffset_play > maxOffset ? maxOffset : scrollOffset_play;
                    } else if (currentMenuState == HELP_MENU) {
                        scrollOffset_help += diff * dragSpeed * (SCROLL_BUTTON_COUNT * MAX - WINDOW_HEIGHT) / (
                            WINDOW_HEIGHT);
                        // 更新滚动位置
                        dragStartY = mouseY;
                        // 限制拖动范围
                        scrollOffset_help = scrollOffset_help < 0 ? 0 : scrollOffset_help;
                        int maxOffset = HELPDRAGHEIGHT;
                        scrollOffset_help = scrollOffset_help > maxOffset ? maxOffset : scrollOffset_help;
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                isDragging = false; // 停止拖动
                dragStartY = -1; // 重置拖动起始位置
            } else if (e.type == SDL_MOUSEWHEEL) {
                // 处理滚轮事件
                if (currentMenuState == HELP_MENU) {
                    int scrollSpeed = 30; // 滚动灵敏度，控制每次滚动的步长
                    if (e.wheel.y > 0) {
                        scrollOffset_help -= scrollSpeed; // 向上滚动
                    } else if (e.wheel.y < 0) {
                        scrollOffset_help += scrollSpeed; // 向下滚动
                    }

                    // 限制滚动范围
                    scrollOffset_help = scrollOffset_help < 0 ? 0 : scrollOffset_help;
                    int maxOffset =
                            (SCROLL_BUTTON_COUNT * MAX - WINDOW_HEIGHT); // 根据当前菜单选择不同的最大滚动值
                    scrollOffset_help = scrollOffset_help > maxOffset ? maxOffset : scrollOffset_help;
                } else if (currentMenuState == SCROLL_MENU) {
                    int scrollSpeed = 30; // 滚动灵敏度，控制每次滚动的步长
                    if (e.wheel.y > 0) {
                        scrollOffset_play -= scrollSpeed; // 向上滚动
                    } else if (e.wheel.y < 0) {
                        scrollOffset_play += scrollSpeed; // 向下滚动
                    }

                    // 限制滚动范围
                    scrollOffset_play = scrollOffset_play < 0 ? 0 : scrollOffset_play;
                    int maxOffset =
                            (SCROLL_BUTTON_COUNT * MAX - WINDOW_HEIGHT); // 根据当前菜单选择不同的最大滚动值
                    scrollOffset_play = scrollOffset_play > maxOffset ? maxOffset : scrollOffset_play;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;



                if (currentMenuState == SCROLL_MENU) {
                    for (int i = 0; i < SCROLL_BUTTON_COUNT; i++) {
                        Button button = {
                            {
                                WINDOW_WIDTH - BUTTON_WIDTH - 20,
                                10 + (i * 60) - scrollOffset_play,
                                BUTTON_WIDTH, BUTTON_HEIGHT
                            },
                            "Button"
                        };
                        if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &button.rect)) {
                            // 右键查看按钮的描述
                            //printf("Right-clicked on Button %d\n", i + 1);
                            popup.visible = true;
                            popup.text = name_skill_explanation[i];
                        }
                    }
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (currentMenuState == MAIN_MENU) {
                    if (e.key.keysym.sym == SDLK_LSHIFT || e.key.keysym.sym == SDLK_RSHIFT) {
                        prequit = true;
                    }
                } else if (currentMenuState == SCROLL_MENU) {
                    if (!popup.visible && e.key.keysym.sym == SDLK_SPACE) {
                        if (!is_online) {
                            if (playercommand != -1 && blacksmith_skill[playercommand].is_valid) {
                                pthread_create(&settle, NULL, settlement, &playercommand);
                            }
                        } else {
                            if (playercommand != -1 && blacksmith_skill[playercommand].is_valid) {
                                sentnumber = playercommand;
                            }
                        }
                    }
                    else if(e.key.keysym.sym == SDLK_EQUALS) {
                        if(playercommand != -1 && blacksmith_skill[playercommand].funcskill(&player, playerplus + 1, 0)) {
                            ++playerplus;
                        }
                    }
                    else if(e.key.keysym.sym == SDLK_MINUS) {
                        playerplus = playerplus - 1 > 0 ? playerplus - 1 : 0;
                    }
                } else if (currentMenuState == JOIN_MENU) {
                    if (e.key.keysym.sym == SDLK_BACKSPACE) {
                        portsite = portsite - 1 > 0 ? portsite - 1 : 0;
                        port[portsite] = '\0';
                    }
                    if (portsite < 5) {
                        if (e.key.keysym.sym == SDLK_0) {
                            port[portsite] = 48;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_1) {
                            port[portsite] = 49;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_2) {
                            port[portsite] = 50;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_3) {
                            port[portsite] = 51;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_4) {
                            port[portsite] = 52;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_5) {
                            port[portsite] = 53;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_6) {
                            port[portsite] = 54;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_7) {
                            port[portsite] = 55;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_8) {
                            port[portsite] = 56;
                            ++portsite;
                            port[portsite] = '\0';
                        } else if (e.key.keysym.sym == SDLK_9) {
                            port[portsite] = 57;
                            ++portsite;
                            port[portsite] = '\0';
                        }
                    }
                    if (portsite == 5) {
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            ++portsite;
                        }
                    }
                }
            }
        }
        // 渲染主菜单
        if (currentMenuState == MAIN_MENU) {
            renderMainMenu();
        }
        // 渲染帮助菜单
        else if (currentMenuState == END_MENU) {
            renderEndMenu();
        } else if (currentMenuState == HELP_MENU) {
            renderHelpMenu();
        }
        // 渲染滚动按钮菜单
        else if (currentMenuState == SCROLL_MENU) {
            renderScrollButtonsMenu();
        } else if (currentMenuState == ONLINE_MENU) {
            renderMultipleMenu();
        } else if (currentMenuState == CREATE_MENU) {
            rendercreateMenu();
        } else if (currentMenuState == JOIN_MENU) {
            renderjoinMenu();
        }
        renderGamerData(renderer, player, robot, currentMenuState);
        SDL_RenderPresent(renderer);
    }
    if (!is_done) {
        pthread_join(settle, NULL);
    }
    closeSDL();
    return 0;
}
