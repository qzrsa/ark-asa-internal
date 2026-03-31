#pragma once

#include "../vendor/CppSDK/SDK/Engine_classes.hpp"
#include "../vendor/CppSDK/SDK/Engine_functions.cpp"

#include <Windows.h>

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <string>

namespace ZeroGUI {
    namespace Input {
        bool mouseDown[5];
        bool mouseDownAlready[256];

        bool keysDown[256];
        bool keysDownAlready[256];

        bool IsAnyMouseDown() {
            if (mouseDown[0])
                return true;
            if (mouseDown[1])
                return true;
            if (mouseDown[2])
                return true;
            if (mouseDown[3])
                return true;
            if (mouseDown[4])
                return true;

            return false;
        }

        bool IsMouseClicked(int button, int element_id, bool repeat) {
            if (mouseDown[button]) {
                if (!mouseDownAlready[element_id]) {
                    mouseDownAlready[element_id] = true;
                    return true;
                }
                if (repeat)
                    return true;
            } else {
                mouseDownAlready[element_id] = false;
            }
            return false;
        }

        bool IsKeyPressed(int key, bool repeat) {
            if (keysDown[key]) {
                if (!keysDownAlready[key]) {
                    keysDownAlready[key] = true;
                    return true;
                }
                if (repeat)
                    return true;
            } else {
                keysDownAlready[key] = false;
            }
            return false;
        }

        void Handle() {
            if (GetAsyncKeyState(0x01))
                mouseDown[0] = true;
            else
                mouseDown[0] = false;
        }
    } // namespace Input
} // namespace ZeroGUI

namespace ZeroGUI {
    SDK::UFont* Font = nullptr;
    float FontScale = 0.5;

    namespace Colors {
        SDK::FLinearColor Text {1.0f, 1.0f, 1.0f, 1.0f};
        SDK::FLinearColor Text_Shadow {0, 0, 0, 1.0f};
        SDK::FLinearColor Text_Outline {0, 0, 0, 1.0f};

        SDK::FLinearColor Window_Background {0.06f, 0.06f, 0.06f, 0.94f};
        SDK::FLinearColor Window_Header {0.14f, 0.14f, 0.14f, 1.0f};

        SDK::FLinearColor Button_Idle {0.16f, 0.29f, 0.48f, 1.0f};
        SDK::FLinearColor Button_Hovered {0.26f, 0.59f, 0.98f, 0.80f};
        SDK::FLinearColor Button_Active {0.26f, 0.59f, 0.98f, 1.0f};

        SDK::FLinearColor Checkbox_Idle {0.16f, 0.29f, 0.48f, 1.0f};
        SDK::FLinearColor Checkbox_Hovered {0.26f, 0.59f, 0.98f, 0.80f};
        SDK::FLinearColor Checkbox_Enabled {1.0f, 1.0f, 1.0f, 1.0f};

        SDK::FLinearColor Combobox_Idle {0.16f, 0.29f, 0.48f, 1.0f};
        SDK::FLinearColor Combobox_Hovered {0.24f, 0.24f, 0.25f, 1.0f};
        SDK::FLinearColor Combobox_Elements {0.12f, 0.12f, 0.12f, 1.0f};

        SDK::FLinearColor Slider_Idle {0.16f, 0.29f, 0.48f, 0.54f};
        SDK::FLinearColor Slider_Hovered {0.24f, 0.24f, 0.25f, 1.0f};
        SDK::FLinearColor Slider_Progress {0.26f, 0.59f, 0.98f, 1.0f};
        SDK::FLinearColor Slider_Button {0.26f, 0.59f, 0.98f, 1.0f};

        SDK::FLinearColor ColorPicker_Background {0.06f, 0.06f, 0.06f, 1.0f};
    } // namespace Colors

    namespace PostRenderer {
        struct DrawList {
            int type = -1; //1 = FilledRect, 2 = TextLeft, 3 = TextCenter, 4 = Draw_Line
            SDK::FVector2D pos;
            SDK::FVector2D size;
            SDK::FLinearColor color;
            wchar_t* name;
            bool outline;

            SDK::FVector2D from;
            SDK::FVector2D to;
            int thickness;
        };

        DrawList drawlist[128];

        void drawFilledRect(SDK::FVector2D pos, float w, float h, SDK::FLinearColor color) {
            for (int i = 0; i < 128; i++) {
                if (drawlist[i].type == -1) {
                    drawlist[i].type = 1;
                    drawlist[i].pos = pos;
                    drawlist[i].size = SDK::FVector2D {w, h};
                    drawlist[i].color = color;
                    return;
                }
            }
        }

        void TextLeft(wchar_t* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline) {
            for (int i = 0; i < 128; i++) {
                if (drawlist[i].type == -1) {
                    drawlist[i].type = 2;
                    drawlist[i].name = name;
                    drawlist[i].pos = pos;
                    drawlist[i].outline = outline;
                    drawlist[i].color = color;
                    return;
                }
            }
        }

        void TextCenter(wchar_t* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline) {
            for (int i = 0; i < 128; i++) {
                if (drawlist[i].type == -1) {
                    drawlist[i].type = 3;
                    drawlist[i].name = name;
                    drawlist[i].pos = pos;
                    drawlist[i].outline = outline;
                    drawlist[i].color = color;
                    return;
                }
            }
        }

        void Draw_Line(SDK::FVector2D from, SDK::FVector2D to, int thickness, SDK::FLinearColor color) {
            for (int i = 0; i < 128; i++) {
                if (drawlist[i].type == -1) {
                    drawlist[i].type = 4;
                    drawlist[i].from = from;
                    drawlist[i].to = to;
                    drawlist[i].thickness = thickness;
                    drawlist[i].color = color;
                    return;
                }
            }
        }
    } // namespace PostRenderer

    SDK::UCanvas* canvas;

    bool hover_element = false;
    SDK::FVector2D menu_pos = SDK::FVector2D {0, 0};
    float offset_x = 0.0f;
    float offset_y = 0.0f;

    SDK::FVector2D first_element_pos = SDK::FVector2D {0, 0};

    SDK::FVector2D last_element_pos = SDK::FVector2D {0, 0};
    SDK::FVector2D last_element_size = SDK::FVector2D {0, 0};

    int current_element = -1;
    SDK::FVector2D current_element_pos = SDK::FVector2D {0, 0};
    SDK::FVector2D current_element_size = SDK::FVector2D {0, 0};
    int elements_count = 0;

    bool sameLine = false;

    bool pushY = false;
    float pushYvalue = 0.0f;

    void SetupCanvas(SDK::UCanvas* _canvas) {
        canvas = _canvas;
    }

    SDK::FVector2D CursorPos() {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        return SDK::FVector2D {(float)cursorPos.x, (float)cursorPos.y};
    }

    bool MouseInZone(SDK::FVector2D pos, SDK::FVector2D size) {
        SDK::FVector2D cursor_pos = CursorPos();

        if (cursor_pos.X > pos.X && cursor_pos.Y > pos.Y)
            if (cursor_pos.X < pos.X + size.X && cursor_pos.Y < pos.Y + size.Y)
                return true;

        return false;
    }

    void Draw_Cursor(bool toogle) {
        if (toogle) {
            SDK::FVector2D cursorPos = CursorPos();
            canvas->K2_DrawLine(
                SDK::FVector2D {cursorPos.X, cursorPos.Y},
                SDK::FVector2D {cursorPos.X + 35, cursorPos.Y + 10},
                1,
                SDK::FLinearColor {0.30f, 0.30f, 0.80f, 1.0f}
            );

            int x = 35;
            int y = 10;
            while (y != 30) //20 steps
            {
                x -= 1;
                if (x < 15)
                    x = 15;
                y += 1;
                if (y > 30)
                    y = 30;

                canvas->K2_DrawLine(
                    SDK::FVector2D {cursorPos.X, cursorPos.Y},
                    SDK::FVector2D {cursorPos.X + x, cursorPos.Y + y},
                    1,
                    SDK::FLinearColor {0.30f, 0.30f, 0.80f, 1.0f}
                );
            }

            canvas->K2_DrawLine(
                SDK::FVector2D {cursorPos.X, cursorPos.Y},
                SDK::FVector2D {cursorPos.X + 15, cursorPos.Y + 30},
                1,
                SDK::FLinearColor {0.30f, 0.30f, 0.80f, 1.0f}
            );
            canvas->K2_DrawLine(
                SDK::FVector2D {cursorPos.X + 35, cursorPos.Y + 10},
                SDK::FVector2D {cursorPos.X + 15, cursorPos.Y + 30},
                1,
                SDK::FLinearColor {0.30f, 0.30f, 0.80f, 1.0f}
            );
        }
    }

    void SameLine() {
        sameLine = true;
    }

    void PushNextElementY(float y, bool from_last_element = true) {
        pushY = true;
        if (from_last_element)
            pushYvalue = last_element_pos.Y + last_element_size.Y + y;
        else
            pushYvalue = y;
    }

    void NextColumn(float x) {
        offset_x = x;
        PushNextElementY(first_element_pos.Y, false);
    }

    void ClearFirstPos() {
        first_element_pos = SDK::FVector2D {0, 0};
    }

    void TextLeft(wchar_t* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline) {
        int length = wcslen(name) + 1;
        canvas->K2_DrawText(
            Font,
            SDK::FString {name, length, length},
            pos,
            SDK::FVector2D {FontScale, FontScale},
            color,
            1.0f,
            Colors::Text_Shadow,
            SDK::FVector2D {0, 0},
            false,
            true,
            true,
            Colors::Text_Outline
        );
    }

    void TextCenter(wchar_t* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline) {
        int length = wcslen(name) + 1;
        canvas->K2_DrawText(
            Font,
            SDK::FString {name, length, length},
            pos,
            SDK::FVector2D {FontScale, FontScale},
            color,
            1.0f,
            Colors::Text_Shadow,
            SDK::FVector2D {0, 0},
            true,
            true,
            true,
            Colors::Text_Outline
        );
    }

    void GetColor(SDK::FLinearColor* color, float* r, float* g, float* b, float* a) {
        *r = color->R;
        *g = color->G;
        *b = color->B;
        *a = color->A;
    }

    UINT32 GetColorUINT(int r, int g, int b, int a) {
        UINT32 result = (BYTE(a) << 24) + (BYTE(r) << 16) + (BYTE(g) << 8) + BYTE(b);
        return result;
    }

    void Draw_Line(SDK::FVector2D from, SDK::FVector2D to, int thickness, SDK::FLinearColor color) {
        canvas->K2_DrawLine(SDK::FVector2D {from.X, from.Y}, SDK::FVector2D {to.X, to.Y}, thickness, color);
    }

    void drawFilledRect(SDK::FVector2D initial_pos, float w, float h, SDK::FLinearColor color) {
        for (float i = 0.0f; i < h; i += 1.0f)
            canvas->K2_DrawLine(
                SDK::FVector2D {initial_pos.X, initial_pos.Y + i},
                SDK::FVector2D {initial_pos.X + w, initial_pos.Y + i},
                1.0f,
                color
            );
    }

    void DrawFilledCircle(SDK::FVector2D pos, float r, SDK::FLinearColor color) {
        float smooth = 0.07f;

        double PI = 3.14159265359;
        int size = (int)(2.0f * PI / smooth) + 1;

        float angle = 0.0f;
        int i = 0;

        for (; angle < 2 * PI; angle += smooth, i++) {
            Draw_Line(
                SDK::FVector2D {pos.X, pos.Y},
                SDK::FVector2D {pos.X + cosf(angle) * r, pos.Y + sinf(angle) * r},
                1.0f,
                color
            );
        }
    }

    void DrawCircle(SDK::FVector2D pos, int radius, int numSides, SDK::FLinearColor Color) {
        float PI = 3.1415927f;

        float Step = PI * 2.0 / numSides;
        int Count = 0;
        SDK::FVector2D V[128];
        for (float a = 0; a < PI * 2.0; a += Step) {
            float X1 = radius * cos(a) + pos.X;
            float Y1 = radius * sin(a) + pos.Y;
            float X2 = radius * cos(a + Step) + pos.X;
            float Y2 = radius * sin(a + Step) + pos.Y;
            V[Count].X = X1;
            V[Count].Y = Y1;
            V[Count + 1].X = X2;
            V[Count + 1].Y = Y2;
            //Draw_Line(SDK::FVector2D{ pos.X, pos.Y }, SDK::FVector2D{ X2, Y2 }, 1.0f, Color); // Points from Centre to ends of circle
            Draw_Line(SDK::FVector2D {V[Count].X, V[Count].Y}, SDK::FVector2D {X2, Y2}, 1.0f,
                      Color); // Circle Around
        }
    }

    SDK::FVector2D dragPos;

    bool Window(wchar_t* name, SDK::FVector2D* pos, SDK::FVector2D size, bool isOpen) {
        elements_count = 0;

        if (!isOpen)
            return false;

        bool isHovered = MouseInZone(SDK::FVector2D {pos->X, pos->Y}, size);

        //Drop last element
        if (current_element != -1 && !GetAsyncKeyState(0x1)) {
            current_element = -1;
        }

        //Drag
        if (hover_element && GetAsyncKeyState(0x1)) {
        } else if ((isHovered || dragPos.X != 0) && !hover_element) {
            if (Input::IsMouseClicked(0, elements_count, true)) {
                SDK::FVector2D cursorPos = CursorPos();

                cursorPos.X -= size.X;
                cursorPos.Y -= size.Y;

                if (dragPos.X == 0) {
                    dragPos.X = (cursorPos.X - pos->X);
                    dragPos.Y = (cursorPos.Y - pos->Y);
                }
                pos->X = cursorPos.X - dragPos.X;
                pos->Y = cursorPos.Y - dragPos.Y;
            } else {
                dragPos = SDK::FVector2D {0, 0};
            }
        } else {
            hover_element = false;
        }

        offset_x = 0.0f;
        offset_y = 0.0f;
        menu_pos = SDK::FVector2D {pos->X, pos->Y};
        first_element_pos = SDK::FVector2D {0, 0};
        current_element_pos = SDK::FVector2D {0, 0};
        current_element_size = SDK::FVector2D {0, 0};

        //Bg
        drawFilledRect(SDK::FVector2D {pos->X, pos->Y}, size.X, size.Y, Colors::Window_Background);
        //drawFilledRect(SDK::FVector2D{ pos->X, pos->Y }, 122, size.Y, SDK::FLinearColor{ 0.006f, 0.006f, 0.006f, 1.0f });//My tabs bg

        //Header
        drawFilledRect(SDK::FVector2D {pos->X, pos->Y}, size.X, 25.0f, Colors::Window_Header);

        offset_y += 25.0f;

        //Title
        SDK::FVector2D titlePos = SDK::FVector2D {pos->X + size.X / 2, pos->Y + 25 / 2};
        TextCenter(name, titlePos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        return true;
    }

    void Text(wchar_t* text, bool center = false, bool outline = false) {
        elements_count++;

        float size = 25;
        SDK::FVector2D padding = SDK::FVector2D {10, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }

        if (!sameLine)
            offset_y += size + padding.Y;

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X + 5.0f, pos.Y + size / 2};
        if (center)
            TextCenter(text, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, outline);
        else
            TextLeft(text, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, outline);

        sameLine = false;
        last_element_pos = pos;
        //last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;
    }

    bool ButtonTab(wchar_t* name, SDK::FVector2D size, bool active) {
        elements_count++;

        SDK::FVector2D padding = SDK::FVector2D {5, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, size);

        //Bg
        if (active) {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Active);
        } else if (isHovered) {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Hovered);
            hover_element = true;
        } else {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Idle);
        }

        if (!sameLine)
            offset_y += size.Y + padding.Y;

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X + size.X / 2, pos.Y + size.Y / 2};
        TextCenter(name, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        sameLine = false;
        last_element_pos = pos;
        last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;

        if (isHovered && Input::IsMouseClicked(0, elements_count, false))
            return true;

        return false;
    }

    bool Button(wchar_t* name, SDK::FVector2D size) {
        elements_count++;

        SDK::FVector2D padding = SDK::FVector2D {5, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, size);

        //Bg
        if (isHovered) {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Hovered);
            hover_element = true;
        } else {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Idle);
        }

        if (!sameLine)
            offset_y += size.Y + padding.Y;

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X + size.X / 2, pos.Y + size.Y / 2};
        //if (!TextOverlapedFromActiveElement(textPos))
        TextCenter(name, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        sameLine = false;
        last_element_pos = pos;
        last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;

        if (isHovered && Input::IsMouseClicked(0, elements_count, false))
            return true;

        return false;
    }

    void Checkbox(wchar_t* name, bool* value) {
        elements_count++;

        float size = 18;
        SDK::FVector2D padding = SDK::FVector2D {10, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, SDK::FVector2D {size, size});

        //Bg
        if (isHovered) {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size, size, Colors::Checkbox_Hovered);
            hover_element = true;
        } else {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size, size, Colors::Checkbox_Idle);
        }

        if (!sameLine)
            offset_y += size + padding.Y;

        if (*value) {
            // 计算钩的三个关键点坐标 (基于当前 pos 和 size)
            // 起点：左中偏下；转折点：底中偏左；终点：右上
            SDK::FVector2D p1 = {pos.X + size * 0.25f, pos.Y + size * 0.45f};
            SDK::FVector2D p2 = {pos.X + size * 0.45f, pos.Y + size * 0.70f};
            SDK::FVector2D p3 = {pos.X + size * 0.80f, pos.Y + size * 0.25f};

            // 使用两条直线绘制出“钩”的形状
            canvas->K2_DrawLine(p1, p2, 2.0f, Colors::Checkbox_Enabled); // 短边
            canvas->K2_DrawLine(p2, p3, 2.0f, Colors::Checkbox_Enabled); // 长边
        }

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X + size + 5.0f, pos.Y + size / 2};
        //if (!TextOverlapedFromActiveElement(textPos))
        TextLeft(name, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        sameLine = false;
        last_element_pos = pos;
        //last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;

        if (isHovered && Input::IsMouseClicked(0, elements_count, false))
            *value = !*value;
    }

    void SliderInt(wchar_t* name, int* value, int min, int max) {
        elements_count++;

        SDK::FVector2D size = SDK::FVector2D {240, 50};
        SDK::FVector2D slider_size = SDK::FVector2D {200, 10};
        SDK::FVector2D padding = SDK::FVector2D {10, 15};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y}, slider_size);

        if (!sameLine)
            offset_y += size.Y + padding.Y;

        //Bg
        if (isHovered || current_element == elements_count) {
            //Drag
            if (Input::IsMouseClicked(0, elements_count, true)) {
                current_element = elements_count;

                SDK::FVector2D cursorPos = CursorPos();
                *value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
                if (*value < min)
                    *value = min;
                if (*value > max)
                    *value = max;
            }

            drawFilledRect(
                SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y},
                slider_size.X,
                slider_size.Y,
                Colors::Slider_Hovered
            );
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f}, 5.0f, 5.0f, Colors::Slider_Progress);

            hover_element = true;
        } else {
            drawFilledRect(
                SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y},
                slider_size.X,
                slider_size.Y,
                Colors::Slider_Idle
            );
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f}, 5.0f, 5.0f, Colors::Slider_Progress);
        }

        //Value
        float oneP = slider_size.X / (max - min);
        drawFilledRect(
            SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y},
            oneP * (*value - min),
            slider_size.Y,
            Colors::Slider_Progress
        );
        //drawFilledRect(SDK::FVector2D{ pos.X + oneP * (*value - min) - 10.0f, pos.Y + slider_size.Y - 5.0f + padding.Y }, 20.0f, 20.0f, Colors::Slider_Button);
        DrawFilledCircle(
            SDK::FVector2D {pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y},
            10.0f,
            Colors::Slider_Button
        );
        DrawFilledCircle(
            SDK::FVector2D {pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y},
            5.0f,
            Colors::Slider_Progress
        );

        wchar_t buffer[32];
        swprintf_s(buffer, L"%i", *value);
        SDK::FVector2D valuePos = SDK::FVector2D {pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 25 + padding.Y};
        TextCenter(buffer, valuePos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X + 5, pos.Y + 10};
        TextLeft(name, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        sameLine = false;
        last_element_pos = pos;
        last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;
    }

    void SliderFloat(wchar_t* name, float* value, float min, float max, wchar_t* format = const_cast<wchar_t*>(L"%.0f")) {
        elements_count++;

        SDK::FVector2D size = SDK::FVector2D {210, 40};
        SDK::FVector2D slider_size = SDK::FVector2D {170, 7};
        SDK::FVector2D adjust_zone = SDK::FVector2D {0, 20};
        SDK::FVector2D padding = SDK::FVector2D {10, 15};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(
            SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y - adjust_zone.Y},
            SDK::FVector2D {slider_size.X, slider_size.Y + adjust_zone.Y * 1.5f}
        );

        if (!sameLine)
            offset_y += size.Y + padding.Y;

        //Bg
        if (isHovered || current_element == elements_count) {
            //Drag
            if (Input::IsMouseClicked(0, elements_count, true)) {
                current_element = elements_count;

                SDK::FVector2D cursorPos = CursorPos();
                *value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
                if (*value < min)
                    *value = min;
                if (*value > max)
                    *value = max;
            }

            drawFilledRect(
                SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y},
                slider_size.X,
                slider_size.Y,
                Colors::Slider_Hovered
            );
            DrawFilledCircle(SDK::FVector2D {pos.X, pos.Y + padding.Y + 9.3f}, 3.1f, Colors::Slider_Progress);
            DrawFilledCircle(SDK::FVector2D {pos.X + slider_size.X, pos.Y + padding.Y + 9.3f}, 3.1f, Colors::Slider_Hovered);

            hover_element = true;
        } else {
            drawFilledRect(
                SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y},
                slider_size.X,
                slider_size.Y,
                Colors::Slider_Idle
            );
            DrawFilledCircle(SDK::FVector2D {pos.X, pos.Y + padding.Y + 9.3f}, 3.1f, Colors::Slider_Progress);
            DrawFilledCircle(SDK::FVector2D {pos.X + slider_size.X, pos.Y + padding.Y + 9.3f}, 3.1f, Colors::Slider_Idle);
        }

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X, pos.Y + 5};
        TextLeft(name, textPos, Colors::Text, false);

        //Value
        float oneP = slider_size.X / (max - min);
        drawFilledRect(
            SDK::FVector2D {pos.X, pos.Y + slider_size.Y + padding.Y},
            oneP * (*value - min),
            slider_size.Y,
            Colors::Slider_Progress
        );
        DrawFilledCircle(
            SDK::FVector2D {pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 2.66f + padding.Y},
            8.0f,
            Colors::Slider_Button
        );
        DrawFilledCircle(
            SDK::FVector2D {pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 2.66f + padding.Y},
            4.0f,
            Colors::Slider_Progress
        );

        wchar_t buffer[32];
        swprintf_s(buffer, format, *value);
        SDK::FVector2D valuePos = SDK::FVector2D {pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 20 + padding.Y};
        TextCenter(buffer, valuePos, Colors::Text, false);

        sameLine = false;
        last_element_pos = pos;
        last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;
    }

    bool checkbox_enabled[256];

    void Combobox(wchar_t* name, SDK::FVector2D size, int* value, const wchar_t* arg, ...) {
        elements_count++;

        SDK::FVector2D padding = SDK::FVector2D {5, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, size);

        //Bg
        if (isHovered || checkbox_enabled[elements_count]) {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Combobox_Hovered);

            hover_element = true;
        } else {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Combobox_Idle);
        }

        if (!sameLine)
            offset_y += size.Y + padding.Y;

        //Text
        SDK::FVector2D textPos = SDK::FVector2D {pos.X + size.X + 5.0f, pos.Y + size.Y / 2};
        TextLeft(name, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

        //Elements
        bool isHovered2 = false;
        SDK::FVector2D element_pos = pos;
        int num = 0;

        if (checkbox_enabled[elements_count]) {
            current_element_size.X = element_pos.X - 5.0f;
            current_element_size.Y = element_pos.Y - 5.0f;
        }
        va_list arguments;
        for (va_start(arguments, arg); arg != NULL; arg = va_arg(arguments, const wchar_t*)) {
            //Selected Element
            if (num == *value) {
                SDK::FVector2D _textPos = SDK::FVector2D {pos.X + size.X / 2, pos.Y + size.Y / 2};
                TextCenter((wchar_t*)arg, _textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);
            }

            if (checkbox_enabled[elements_count]) {
                element_pos.Y += 25.0f;

                isHovered2 = MouseInZone(SDK::FVector2D {element_pos.X, element_pos.Y}, SDK::FVector2D {size.X, 25.0f});
                if (isHovered2) {
                    hover_element = true;
                    PostRenderer::drawFilledRect(
                        SDK::FVector2D {element_pos.X, element_pos.Y},
                        size.X,
                        25.0f,
                        Colors::Combobox_Hovered
                    );

                    //Click
                    if (Input::IsMouseClicked(0, elements_count, false)) {
                        *value = num;
                        checkbox_enabled[elements_count] = false;
                    }
                } else {
                    PostRenderer::drawFilledRect(
                        SDK::FVector2D {element_pos.X, element_pos.Y},
                        size.X,
                        25.0f,
                        Colors::Combobox_Idle
                    );
                }

                PostRenderer::TextLeft(
                    (wchar_t*)arg,
                    SDK::FVector2D {element_pos.X + 5.0f, element_pos.Y + 15.0f},
                    SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f},
                    false
                );
            }
            num++;
        }
        va_end(arguments);
        if (checkbox_enabled[elements_count]) {
            current_element_size.X = element_pos.X + 5.0f;
            current_element_size.Y = element_pos.Y + 5.0f;
        }

        sameLine = false;
        last_element_pos = pos;
        last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;

        if (isHovered && Input::IsMouseClicked(0, elements_count, false)) {
            checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
        }
        if (!isHovered && !isHovered2 && Input::IsMouseClicked(0, elements_count, false)) {
            checkbox_enabled[elements_count] = false;
        }
    }

    int active_hotkey = -1;
    bool already_pressed = false;

    std::wstring VirtualKeyCodeToString(UCHAR virtualKey) {
        UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

        if (virtualKey == VK_LBUTTON) {
            return L"MOUSE0";
        }
        if (virtualKey == VK_RBUTTON) {
            return L"MOUSE1";
        }
        if (virtualKey == VK_MBUTTON) {
            return L"MBUTTON";
        }
        if (virtualKey == VK_XBUTTON1) {
            return L"XBUTTON1";
        }
        if (virtualKey == VK_XBUTTON2) {
            return L"XBUTTON2";
        }

        wchar_t szName[128];
        int result = 0;
        switch (virtualKey) {
            case VK_LEFT:
            case VK_UP:
            case VK_RIGHT:
            case VK_DOWN:
            case VK_RCONTROL:
            case VK_RMENU:
            case VK_LWIN:
            case VK_RWIN:
            case VK_APPS:
            case VK_PRIOR:
            case VK_NEXT:
            case VK_END:
            case VK_HOME:
            case VK_INSERT:
            case VK_DELETE:
            case VK_DIVIDE:
            case VK_NUMLOCK:
                scanCode |= KF_EXTENDED;
            default:
                result = GetKeyNameTextW(scanCode << 16, szName, 128);
        }

        return szName;
    }

    void Hotkey(SDK::FVector2D size, int* key) {
        elements_count++;

        SDK::FVector2D padding = SDK::FVector2D {5, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y + (last_element_size.Y / 2) - size.Y / 2;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, size);

        //Bg
        if (isHovered) {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Hovered);
            hover_element = true;
        } else {
            drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, Colors::Button_Idle);
        }

        if (!sameLine)
            offset_y += size.Y + padding.Y;

        if (active_hotkey == elements_count) {
            //Text
            SDK::FVector2D textPos = SDK::FVector2D {pos.X + size.X / 2, pos.Y + size.Y / 2};
            TextCenter(const_cast<wchar_t*>(L"[Press Key]"), textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

            if (!ZeroGUI::Input::IsAnyMouseDown()) {
                already_pressed = false;
            }

            if (!already_pressed) {
                for (int code = 0; code < 255; code++) {
                    if (GetAsyncKeyState(code)) {
                        *key = code;
                        active_hotkey = -1;
                    }
                }
            }
        } else {
            //Text
            SDK::FVector2D textPos = SDK::FVector2D {pos.X + size.X / 2, pos.Y + size.Y / 2};
            TextCenter(
                (wchar_t*)VirtualKeyCodeToString(*key).c_str(),
                textPos,
                SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f},
                false
            );

            if (isHovered) {
                if (Input::IsMouseClicked(0, elements_count, false)) {
                    already_pressed = true;
                    active_hotkey = elements_count;

                    //Queue Fix
                    for (int code = 0; code < 255; code++)
                        if (GetAsyncKeyState(code)) {}
                }
            } else {
                if (Input::IsMouseClicked(0, elements_count, false)) {
                    active_hotkey = -1;
                }
            }
        }

        sameLine = false;
        last_element_pos = pos;
        last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;
    }

    int active_picker = -1;
    SDK::FLinearColor saved_color;

    bool ColorPixel(SDK::FVector2D pos, SDK::FVector2D size, SDK::FLinearColor* original, SDK::FLinearColor color) {
        PostRenderer::drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size.X, size.Y, color);

        //Выбранный цвет
        if (original->R == color.R && original->G == color.G && original->B == color.B) {
            PostRenderer::Draw_Line(
                SDK::FVector2D {pos.X, pos.Y},
                SDK::FVector2D {pos.X + size.X - 1, pos.Y},
                1.0f,
                SDK::FLinearColor {0.0f, 0.0f, 0.0f, 1.0f}
            );
            PostRenderer::Draw_Line(
                SDK::FVector2D {pos.X, pos.Y + size.Y - 1},
                SDK::FVector2D {pos.X + size.X - 1, pos.Y + size.Y - 1},
                1.0f,
                SDK::FLinearColor {0.0f, 0.0f, 0.0f, 1.0f}
            );
            PostRenderer::Draw_Line(
                SDK::FVector2D {pos.X, pos.Y},
                SDK::FVector2D {pos.X, pos.Y + size.Y - 1},
                1.0f,
                SDK::FLinearColor {0.0f, 0.0f, 0.0f, 1.0f}
            );
            PostRenderer::Draw_Line(
                SDK::FVector2D {pos.X + size.X - 1, pos.Y},
                SDK::FVector2D {pos.X + size.X - 1, pos.Y + size.Y - 1},
                1.0f,
                SDK::FLinearColor {0.0f, 0.0f, 0.0f, 1.0f}
            );
        }

        //Смена цвета
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, size);
        if (isHovered) {
            if (Input::IsMouseClicked(0, elements_count, false))
                *original = color;
        }

        return true;
    }

    void ColorPicker(wchar_t* name, SDK::FLinearColor* color) {
        elements_count++;

        float size = 23;
        SDK::FVector2D padding = SDK::FVector2D {10, 10};
        SDK::FVector2D pos = SDK::FVector2D {menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y};
        if (sameLine) {
            pos.X = last_element_pos.X + last_element_size.X + padding.X;
            pos.Y = last_element_pos.Y;
        }
        if (pushY) {
            pos.Y = pushYvalue;
            pushY = false;
            pushYvalue = 0.0f;
            offset_y = pos.Y - menu_pos.Y;
        }
        bool isHovered = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, SDK::FVector2D {size, size});

        if (!sameLine)
            offset_y += size + padding.Y;

        if (active_picker == elements_count) {
            hover_element = true;

            float sizePickerX = 250;
            float sizePickerY = 250;
            bool isHoveredPicker = MouseInZone(SDK::FVector2D {pos.X, pos.Y}, SDK::FVector2D {sizePickerX, sizePickerY - 60});

            //Background
            PostRenderer::drawFilledRect(
                SDK::FVector2D {pos.X, pos.Y},
                sizePickerX,
                sizePickerY - 65,
                Colors::ColorPicker_Background
            );

            //float pixedSize = sizePickerY / pixels;
            //SDK::FLinearColor temp_color{1.0f, 1.0f, 1.0f, 1.0f};
            //float iterator = 0.0f;
            //
            //for (int y = 0; y < pixels; y++)
            //{
            //	for (int x = 0; x < pixels; x++)
            //	{
            //		ColorPixel(SDK::FVector2D{ pos.X + pixedSize * x, pos.Y + pixedSize * y }, pixedSize, color, temp_color);
            //		temp_color.R -= (1.0f - saved_color.R) / pixels;
            //		temp_color.G -= (1.0f - saved_color.G) / pixels;
            //		temp_color.B -= (1.0f - saved_color.B) / pixels;
            //	}
            //
            //	iterator += 1.0f / pixels;
            //	temp_color = SDK::FLinearColor{ 1.0f - iterator, 1.0f - iterator, 1.0f - iterator, 1.0f };
            //}

            SDK::FVector2D pixelSize = SDK::FVector2D {sizePickerX / 12, sizePickerY / 12};

            //0
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {174 / 255.f, 235 / 255.f, 253 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {136 / 255.f, 225 / 255.f, 251 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {108 / 255.f, 213 / 255.f, 250 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {89 / 255.f, 175 / 255.f, 213 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {76 / 255.f, 151 / 255.f, 177 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {60 / 255.f, 118 / 255.f, 140 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {43 / 255.f, 85 / 255.f, 100 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {32 / 255.f, 62 / 255.f, 74 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {255 / 255.f, 255 / 255.f, 255 / 255.f, 1.0f}
                );
            }
            //1
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {175 / 255.f, 205 / 255.f, 252 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {132 / 255.f, 179 / 255.f, 252 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {90 / 255.f, 152 / 255.f, 250 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {55 / 255.f, 120 / 255.f, 250 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {49 / 255.f, 105 / 255.f, 209 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {38 / 255.f, 83 / 255.f, 165 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {28 / 255.f, 61 / 255.f, 120 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {20 / 255.f, 43 / 255.f, 86 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {247 / 255.f, 247 / 255.f, 247 / 255.f, 1.0f}
                );
            }
            //2
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {153 / 255.f, 139 / 255.f, 250 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {101 / 255.f, 79 / 255.f, 249 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {64 / 255.f, 50 / 255.f, 230 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {54 / 255.f, 38 / 255.f, 175 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {39 / 255.f, 31 / 255.f, 144 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {32 / 255.f, 25 / 255.f, 116 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {21 / 255.f, 18 / 255.f, 82 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {16 / 255.f, 13 / 255.f, 61 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {228 / 255.f, 228 / 255.f, 228 / 255.f, 1.0f}
                );
            }
            //3
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {194 / 255.f, 144 / 255.f, 251 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {165 / 255.f, 87 / 255.f, 249 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {142 / 255.f, 57 / 255.f, 239 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {116 / 255.f, 45 / 255.f, 184 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {92 / 255.f, 37 / 255.f, 154 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {73 / 255.f, 29 / 255.f, 121 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {53 / 255.f, 21 / 255.f, 88 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {37 / 255.f, 15 / 255.f, 63 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {203 / 255.f, 203 / 255.f, 203 / 255.f, 1.0f}
                );
            }
            //4
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {224 / 255.f, 162 / 255.f, 197 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {210 / 255.f, 112 / 255.f, 166 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {199 / 255.f, 62 / 255.f, 135 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {159 / 255.f, 49 / 255.f, 105 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {132 / 255.f, 41 / 255.f, 89 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {104 / 255.f, 32 / 255.f, 71 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {75 / 255.f, 24 / 255.f, 51 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {54 / 255.f, 14 / 255.f, 36 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {175 / 255.f, 175 / 255.f, 175 / 255.f, 1.0f}
                );
            }
            //5
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {235 / 255.f, 175 / 255.f, 176 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {227 / 255.f, 133 / 255.f, 135 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {219 / 255.f, 87 / 255.f, 88 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {215 / 255.f, 50 / 255.f, 36 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {187 / 255.f, 25 / 255.f, 7 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {149 / 255.f, 20 / 255.f, 6 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {107 / 255.f, 14 / 255.f, 4 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {77 / 255.f, 9 / 255.f, 3 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {144 / 255.f, 144 / 255.f, 144 / 255.f, 1.0f}
                );
            }
            //6
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {241 / 255.f, 187 / 255.f, 171 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {234 / 255.f, 151 / 255.f, 126 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {229 / 255.f, 115 / 255.f, 76 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {227 / 255.f, 82 / 255.f, 24 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {190 / 255.f, 61 / 255.f, 15 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {150 / 255.f, 48 / 255.f, 12 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {107 / 255.f, 34 / 255.f, 8 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {79 / 255.f, 25 / 255.f, 6 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {113 / 255.f, 113 / 255.f, 113 / 255.f, 1.0f}
                );
            }
            //7
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {245 / 255.f, 207 / 255.f, 169 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {240 / 255.f, 183 / 255.f, 122 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {236 / 255.f, 159 / 255.f, 74 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {234 / 255.f, 146 / 255.f, 37 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {193 / 255.f, 111 / 255.f, 28 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {152 / 255.f, 89 / 255.f, 22 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {110 / 255.f, 64 / 255.f, 16 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {80 / 255.f, 47 / 255.f, 12 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {82 / 255.f, 82 / 255.f, 82 / 255.f, 1.0f}
                );
            }
            //8
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {247 / 255.f, 218 / 255.f, 170 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {244 / 255.f, 200 / 255.f, 124 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {241 / 255.f, 182 / 255.f, 77 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {239 / 255.f, 174 / 255.f, 44 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {196 / 255.f, 137 / 255.f, 34 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {154 / 255.f, 108 / 255.f, 27 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {111 / 255.f, 77 / 255.f, 19 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {80 / 255.f, 56 / 255.f, 14 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {54 / 255.f, 54 / 255.f, 54 / 255.f, 1.0f}
                );
            }
            //9
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {254 / 255.f, 243 / 255.f, 187 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {253 / 255.f, 237 / 255.f, 153 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {253 / 255.f, 231 / 255.f, 117 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {254 / 255.f, 232 / 255.f, 85 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {242 / 255.f, 212 / 255.f, 53 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {192 / 255.f, 169 / 255.f, 42 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {138 / 255.f, 120 / 255.f, 30 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {101 / 255.f, 87 / 255.f, 22 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {29 / 255.f, 29 / 255.f, 29 / 255.f, 1.0f}
                );
            }
            //10
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {247 / 255.f, 243 / 255.f, 185 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {243 / 255.f, 239 / 255.f, 148 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {239 / 255.f, 232 / 255.f, 111 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {235 / 255.f, 229 / 255.f, 76 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {208 / 255.f, 200 / 255.f, 55 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {164 / 255.f, 157 / 255.f, 43 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {118 / 255.f, 114 / 255.f, 31 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {86 / 255.f, 82 / 255.f, 21 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {9 / 255.f, 9 / 255.f, 9 / 255.f, 1.0f}
                );
            }
            //11
            {
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 0},
                    pixelSize,
                    color,
                    SDK::FLinearColor {218 / 255.f, 232 / 255.f, 182 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 1},
                    pixelSize,
                    color,
                    SDK::FLinearColor {198 / 255.f, 221 / 255.f, 143 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 2},
                    pixelSize,
                    color,
                    SDK::FLinearColor {181 / 255.f, 210 / 255.f, 103 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 3},
                    pixelSize,
                    color,
                    SDK::FLinearColor {154 / 255.f, 186 / 255.f, 76 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 4},
                    pixelSize,
                    color,
                    SDK::FLinearColor {130 / 255.f, 155 / 255.f, 64 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 5},
                    pixelSize,
                    color,
                    SDK::FLinearColor {102 / 255.f, 121 / 255.f, 50 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 6},
                    pixelSize,
                    color,
                    SDK::FLinearColor {74 / 255.f, 88 / 255.f, 36 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 7},
                    pixelSize,
                    color,
                    SDK::FLinearColor {54 / 255.f, 64 / 255.f, 26 / 255.f, 1.0f}
                );
                ColorPixel(
                    SDK::FVector2D {pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 8},
                    pixelSize,
                    color,
                    SDK::FLinearColor {0 / 255.f, 0 / 255.f, 0 / 255.f, 1.0f}
                );
            }

            if (isHoveredPicker) {
                if (Input::IsMouseClicked(0, elements_count, false)) {}
            } else {
                if (Input::IsMouseClicked(0, elements_count, false)) {
                    active_picker = -1;
                    //hover_element = false;
                }
            }
        } else {
            //Bg
            if (isHovered) {
                drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size, size, Colors::Checkbox_Hovered);
                hover_element = true;
            } else {
                drawFilledRect(SDK::FVector2D {pos.X, pos.Y}, size, size, Colors::Checkbox_Idle);
            }

            //Color
            drawFilledRect(SDK::FVector2D {pos.X + 4, pos.Y + 4}, size - 8, size - 8, *color);

            //Text
            SDK::FVector2D textPos = SDK::FVector2D {pos.X + size + 5.0f, pos.Y + size / 2};
            TextLeft(name, textPos, SDK::FLinearColor {1.0f, 1.0f, 1.0f, 1.0f}, false);

            if (isHovered && Input::IsMouseClicked(0, elements_count, false)) {
                saved_color = *color;
                active_picker = elements_count;
            }
        }

        sameLine = false;
        last_element_pos = pos;
        //last_element_size = size;
        if (first_element_pos.X == 0.0f)
            first_element_pos = pos;
    }

    void Render() {
        for (int i = 0; i < 128; i++) {
            if (PostRenderer::drawlist[i].type != -1) {
                //Filled Rect
                if (PostRenderer::drawlist[i].type == 1) {
                    ZeroGUI::drawFilledRect(
                        PostRenderer::drawlist[i].pos,
                        PostRenderer::drawlist[i].size.X,
                        PostRenderer::drawlist[i].size.Y,
                        PostRenderer::drawlist[i].color
                    );
                }
                //TextLeft
                else if (PostRenderer::drawlist[i].type == 2) {
                    ZeroGUI::TextLeft(
                        PostRenderer::drawlist[i].name,
                        PostRenderer::drawlist[i].pos,
                        PostRenderer::drawlist[i].color,
                        PostRenderer::drawlist[i].outline
                    );
                }
                //TextCenter
                else if (PostRenderer::drawlist[i].type == 3) {
                    ZeroGUI::TextCenter(
                        PostRenderer::drawlist[i].name,
                        PostRenderer::drawlist[i].pos,
                        PostRenderer::drawlist[i].color,
                        PostRenderer::drawlist[i].outline
                    );
                }
                //Draw_Line
                else if (PostRenderer::drawlist[i].type == 4) {
                    Draw_Line(
                        PostRenderer::drawlist[i].from,
                        PostRenderer::drawlist[i].to,
                        PostRenderer::drawlist[i].thickness,
                        PostRenderer::drawlist[i].color
                    );
                }

                PostRenderer::drawlist[i].type = -1;
            }
        }
    }
} // namespace ZeroGUI
