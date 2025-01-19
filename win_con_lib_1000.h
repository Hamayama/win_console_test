#ifndef WIN_CON_LIB_1000_H
#define WIN_CON_LIB_1000_H

// win_con_lib_1000
// 2025-1-19 v1.04

#include <windows.h>
#include <stdio.h>

#ifndef DISABLE_WIN_CON_DBG
#define USE_WIN_CON_DBG
#endif

#ifndef WIN_CON_DBG_OUTPUT_FILE
#define WIN_CON_DBG_OUTPUT_FILE stderr
#endif

#ifdef USE_WIN_CON_DBG
#define WIN_CON_DBG_PRINT(...)  fprintf(WIN_CON_DBG_OUTPUT_FILE, __VA_ARGS__)
#define WIN_CON_DISP_LAST_ERR() disp_last_err_msg()
#else
#define WIN_CON_DBG_PRINT(...)  do { } while (0)
#define WIN_CON_DISP_LAST_ERR() do { } while (0)
#endif

static void disp_last_err_msg(void)
{
    int err_code;
    char *err_msg = NULL;

    // display last error message
    err_code = GetLastError();
    if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       err_code,
                       MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                       (LPSTR)&err_msg,
                       0,
                       NULL)) {
        WIN_CON_DBG_PRINT("error code=%d : %s\n", err_code, err_msg);
    } else {
        WIN_CON_DBG_PRINT("error code=%d : (no message)\n", err_code);
    }
    LocalFree(err_msg);
}

// console functions
BOOL init_screen(DWORD *con_input_mode_orig, DWORD *con_output_mode_orig);
BOOL end_screen(int restore_on, DWORD con_input_mode_orig, DWORD con_output_mode_orig);
BOOL clear_screen(void);
BOOL get_screen_size(int *w, int *h);
BOOL set_foreground_color_rgb24(int r, int g, int b);
BOOL set_background_color_rgb24(int r, int g, int b);
BOOL set_bold(int on);
BOOL set_italic(int on);
BOOL set_underline(int on);
BOOL set_reverse(int on);
BOOL get_cursor_position(int *x, int *y);
BOOL set_cursor_position(int  x, int  y);
BOOL set_cursor_visible(int on);
BOOL write_console_utf8(char *str);
BOOL write_console_utf16(WCHAR *wstr);
BOOL read_console_input(WORD *event_type,
                        BOOL *kdown, WORD *repeat, WORD *vkey, WORD *vscan, WCHAR *uchar, DWORD *ctrl,
                        SHORT *mouse_x, SHORT *mouse_y, DWORD *mouse_button, DWORD *mouse_ctrl, DWORD *mouse_event_flag);

// init screen
BOOL init_screen(DWORD *con_input_mode_orig, DWORD *con_output_mode_orig)
{
    HANDLE hin  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

    // get original console mode
    if (!GetConsoleMode(hin,  con_input_mode_orig)) {
        WIN_CON_DBG_PRINT("init_screen : GetConsoleMode (1st) failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    if (!GetConsoleMode(hout, con_output_mode_orig)) {
        WIN_CON_DBG_PRINT("init_screen : GetConsoleMode (2nd) failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }

    // set console mode
    // (to set ENABLE_MOUSE_INPUT to on, we must set ENABLE_QUICK_EDIT_MODE to off)
    if (!SetConsoleMode(hin,  ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT)) {
        WIN_CON_DBG_PRINT("init_screen : SetConsoleMode (1st) failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    // (enable vt escape sequences)
    if (!SetConsoleMode(hout, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        WIN_CON_DBG_PRINT("init_screen : SetConsoleMode (2nd) failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    return TRUE;
}

// end screen
BOOL end_screen(int restore_on, DWORD con_input_mode_orig, DWORD con_output_mode_orig)
{
    HANDLE hin  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

    // restore original console mode
    if (restore_on != 0) {
        if (!SetConsoleMode(hin,  con_input_mode_orig)) {
            WIN_CON_DBG_PRINT("end_screen : SetConsoleMode (1st) failed.\n");
            WIN_CON_DISP_LAST_ERR();
            return FALSE;
        }
        if (!SetConsoleMode(hout, con_output_mode_orig)) {
            WIN_CON_DBG_PRINT("end_screen : SetConsoleMode (2nd) failed.\n");
            WIN_CON_DISP_LAST_ERR();
            return FALSE;
        }
    }
    return TRUE;
}

// clear screen
BOOL clear_screen(void)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen_buf_info;
    DWORD attr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    COORD pos = {0, 0};
    DWORD n1, n2;

    // clear screen
    if (!GetConsoleScreenBufferInfo(hout, &screen_buf_info)) {
        WIN_CON_DBG_PRINT("clear_screen : GetConsoleScreenBufferInfo failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    n1 = screen_buf_info.dwSize.X * screen_buf_info.dwSize.Y;
    if (!FillConsoleOutputAttribute(hout, attr, n1, pos, &n2)) {
        WIN_CON_DBG_PRINT("clear_screen : FillConsoleOutputAttribute failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    if (!FillConsoleOutputCharacterW(hout, L' ', n1, pos, &n2)) {
        WIN_CON_DBG_PRINT("clear_screen : FillConsoleOutputCharacterW failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }

    // set cursor to (0, 0)
    set_cursor_position(0, 0);
    return TRUE;
}

// get screen size
BOOL get_screen_size(int *w, int *h)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen_buf_info;

    if (!GetConsoleScreenBufferInfo(hout, &screen_buf_info)) {
        WIN_CON_DBG_PRINT("get_screen_size : GetConsoleScreenBufferInfo failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    *w = screen_buf_info.srWindow.Right  - screen_buf_info.srWindow.Left + 1;
    *h = screen_buf_info.srWindow.Bottom - screen_buf_info.srWindow.Top  + 1;
    return TRUE;
}

// set forground color (RGB 24-bit color)
BOOL set_foreground_color_rgb24(int r, int g, int b)
{
    char vt_buf[100];
    int n;
    int r1, g1, b1;

    if (r < 0) { r1 = 0; } else if (r > 255) { r1 = 255; } else { r1 = r; }
    if (g < 0) { g1 = 0; } else if (g > 255) { g1 = 255; } else { g1 = g; }
    if (b < 0) { b1 = 0; } else if (b > 255) { b1 = 255; } else { b1 = b; }
    n = sprintf_s(vt_buf, 100, "\x1b[38;2;%d;%d;%dm", r1, g1, b1);
    if (n == -1) {
        WIN_CON_DBG_PRINT("set_foreground_color_rgb24 : sprintf_s failed.\n");
        return FALSE;
    }
    write_console_utf8(vt_buf);
    return TRUE;
}

// set background color (RGB 24-bit color)
BOOL set_background_color_rgb24(int r, int g, int b)
{
    char vt_buf[100];
    int n;
    int r1, g1, b1;

    if (r < 0) { r1 = 0; } else if (r > 255) { r1 = 255; } else { r1 = r; }
    if (g < 0) { g1 = 0; } else if (g > 255) { g1 = 255; } else { g1 = g; }
    if (b < 0) { b1 = 0; } else if (b > 255) { b1 = 255; } else { b1 = b; }
    n = sprintf_s(vt_buf, 100, "\x1b[48;2;%d;%d;%dm", r1, g1, b1);
    if (n == -1) {
        WIN_CON_DBG_PRINT("set_background_color_rgb24 : sprintf_s failed.\n");
        return FALSE;
    }
    write_console_utf8(vt_buf);
    return TRUE;
}

// set bold
BOOL set_bold(int on)
{
    char vt_buf[100];
    int n;
    int bold = (on != 0 ? 1 : 22);

    n = sprintf_s(vt_buf, 100, "\x1b[%dm", bold);
    if (n == -1) {
        WIN_CON_DBG_PRINT("set_bold : sprintf_s failed.\n");
        return FALSE;
    }
    write_console_utf8(vt_buf);
    return TRUE;
}

// set italic
BOOL set_italic(int on)
{
    char vt_buf[100];
    int n;
    int italic = (on != 0 ? 3 : 23);

    n = sprintf_s(vt_buf, 100, "\x1b[%dm", italic);
    if (n == -1) {
        WIN_CON_DBG_PRINT("set_italic : sprintf_s failed.\n");
        return FALSE;
    }
    write_console_utf8(vt_buf);
    return TRUE;
}

// set underline
BOOL set_underline(int on)
{
    char vt_buf[100];
    int n;
    int underline = (on != 0 ? 4 : 24);

    n = sprintf_s(vt_buf, 100, "\x1b[%dm", underline);
    if (n == -1) {
        WIN_CON_DBG_PRINT("set_underline : sprintf_s failed.\n");
        return FALSE;
    }
    write_console_utf8(vt_buf);
    return TRUE;
}

// set reverse
BOOL set_reverse(int on)
{
    char vt_buf[100];
    int n;
    int reverse = (on != 0 ? 7 : 27);

    n = sprintf_s(vt_buf, 100, "\x1b[%dm", reverse);
    if (n == -1) {
        WIN_CON_DBG_PRINT("set_reverse : sprintf_s failed.\n");
        return FALSE;
    }
    write_console_utf8(vt_buf);
    return TRUE;
}

// get cursor position
BOOL get_cursor_position(int *x, int *y)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen_buf_info;

    if (!GetConsoleScreenBufferInfo(hout, &screen_buf_info)) {
        WIN_CON_DBG_PRINT("get_cursor_position : GetConsoleScreenBufferInfo failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    *x = screen_buf_info.dwCursorPosition.X;
    *y = screen_buf_info.dwCursorPosition.Y;
    return TRUE;
}

// set cursor position
BOOL set_cursor_position(int  x, int  y)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};

    if (!SetConsoleCursorPosition(hout, pos)) {
        WIN_CON_DBG_PRINT("set_cursor_position : SetConsoleCursorPosition failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    return TRUE;
}

// set cursor visible
BOOL set_cursor_visible(int on)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor_info;

    if (!GetConsoleCursorInfo(hout, &cursor_info)) {
        WIN_CON_DBG_PRINT("set_cursor_visible : GetConsoleCursorInfo failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    cursor_info.bVisible = (on != 0 ? TRUE : FALSE);
    if (!SetConsoleCursorInfo(hout, &cursor_info)) {
        WIN_CON_DBG_PRINT("set_cursor_visible : SetConsoleCursorInfo failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    return TRUE;
}

// write utf-8 string to console
BOOL write_console_utf8(char *str)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD n1, n2, n3;
    WCHAR *wchar_buf = NULL;

    // convert utf-8 string to wide character string
    n1 = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (n1 == 0) {
        WIN_CON_DBG_PRINT("write_console_utf8 : MultiByteToWideChar (1st) failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    wchar_buf = (WCHAR*)malloc(n1 * sizeof(WCHAR));
    if (wchar_buf == NULL) {
        WIN_CON_DBG_PRINT("write_console_utf8 : malloc failed.\n");
        return FALSE;
    }
    n2 = MultiByteToWideChar(CP_UTF8, 0, str, -1, wchar_buf, n1);
    if (n2 == 0) {
        WIN_CON_DBG_PRINT("write_console_utf8 : MultiByteToWideChar (2nd) failed.\n");
        WIN_CON_DISP_LAST_ERR();
        free(wchar_buf);
        return FALSE;
    }

    // write wide character string to console
    if (!WriteConsoleW(hout, wchar_buf, (n2 - 1), &n3, NULL)) {
        WIN_CON_DBG_PRINT("write_console_utf8 : WriteConsoleW failed.\n");
        WIN_CON_DISP_LAST_ERR();
        free(wchar_buf);
        return FALSE;
    }
    free(wchar_buf);
    return TRUE;
}

// write utf-16 string to console
BOOL write_console_utf16(WCHAR *wstr)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD n1, n2;

    // write wide character string to console
    n1 = wcslen(wstr);
    if (!WriteConsoleW(hout, wstr, n1, &n2, NULL)) {
        WIN_CON_DBG_PRINT("write_console_utf16 : WriteConsoleW failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    return TRUE;
}

// read console input (uchar is utf-16 character)
BOOL read_console_input(WORD *event_type,
                        BOOL *kdown, WORD *repeat, WORD *vkey, WORD *vscan, WCHAR *uchar, DWORD *ctrl,
                        SHORT *mouse_x, SHORT *mouse_y, DWORD *mouse_button, DWORD *mouse_ctrl, DWORD *mouse_event_flag)
{
    HANDLE hin  = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD input_rec;
    DWORD event_num;
    DWORD read_event_num;

    // init return values
    *event_type = 0;
    *kdown = FALSE;
    *repeat = 0;
    *vkey = 0;
    *vscan = 0;
    *uchar = 0;
    *ctrl = 0;
    *mouse_x = 0;
    *mouse_y = 0;
    *mouse_button = 0;
    *mouse_ctrl = 0;
    *mouse_event_flag = 0;

    // get number of input events
    if (!GetNumberOfConsoleInputEvents(hin, &event_num)) {
        WIN_CON_DBG_PRINT("read_console_input : GetNumberOfConsoleInputEvents failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    if (event_num == 0) {
        // no input
        return TRUE;
    }

    // read console input
    if (!ReadConsoleInputW(hin, &input_rec, 1, &read_event_num)) {
        WIN_CON_DBG_PRINT("read_console_input : ReadConsoleInputW failed.\n");
        WIN_CON_DISP_LAST_ERR();
        return FALSE;
    }
    if (read_event_num == 0) {
        WIN_CON_DBG_PRINT("read_console_input : ReadConsoleInputW returned before read.\n");
        return FALSE;
    }

    // process event
    switch (input_rec.EventType) {
        case KEY_EVENT:
            *event_type = KEY_EVENT;
            *kdown  = input_rec.Event.KeyEvent.bKeyDown;
            *repeat = input_rec.Event.KeyEvent.wRepeatCount;
            *vkey   = input_rec.Event.KeyEvent.wVirtualKeyCode;
            *vscan  = input_rec.Event.KeyEvent.wVirtualScanCode;
            *uchar  = input_rec.Event.KeyEvent.uChar.UnicodeChar;
            *ctrl   = input_rec.Event.KeyEvent.dwControlKeyState;
            break;

        case MOUSE_EVENT:
            *event_type = MOUSE_EVENT;
            *mouse_x          = input_rec.Event.MouseEvent.dwMousePosition.X;
            *mouse_y          = input_rec.Event.MouseEvent.dwMousePosition.Y;
            *mouse_button     = input_rec.Event.MouseEvent.dwButtonState;
            *mouse_ctrl       = input_rec.Event.MouseEvent.dwControlKeyState;
            *mouse_event_flag = input_rec.Event.MouseEvent.dwEventFlags;
            break;

        case WINDOW_BUFFER_SIZE_EVENT:
            *event_type = WINDOW_BUFFER_SIZE_EVENT;
            break;
    }
    return TRUE;
}

#endif // WIN_CON_LIB_1000_H
