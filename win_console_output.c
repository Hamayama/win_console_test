
// win_console_output
// 2025-1-16 v1.01

#include "win_con_lib_1000.h"

int main(void)
{
    DWORD con_input_mode_orig;
    DWORD con_output_mode_orig;
    int x, y;
    int width, height;
    char str_buf[100];
    int i, i2, i3;
    int color_table[][3] = {
        {  0,   0,   0},
        {  0,   0, 128},
        {  0, 128,   0},
        {  0, 128, 128},
        {128,   0,   0},
        {128,   0, 128},
        {128, 128,   0},
        {192, 192, 192},
        {128, 128, 128},
        {  0,   0, 255},
        {  0, 255,   0},
        {  0, 255, 255},
        {255,   0,   0},
        {255,   0, 255},
        {255, 255,   0},
        {255, 255, 255},
        {255, 215,   0}, // gold
        {255, 165,   0}, // orange
        {255,  99,  71}, // tomato
    };
    int color_table_num = sizeof(color_table) / sizeof(color_table[0]);

    // init screen
    init_screen(&con_input_mode_orig, &con_output_mode_orig);

    // display test 1
    clear_screen();
    write_console_utf8(  "あいうえお");
    write_console_utf16(L"かきくけこ");
    write_console_utf8(  "さしすせそ");

    set_cursor_position( 0,  2);
    write_console_utf8("123");
    write_console_utf8("ABC");

    set_cursor_position( 0,  3);
    write_console_utf8("123\u200BABC"); // ゼロ幅スペースは半角幅になるもよう

    set_cursor_position(10,  0);
    write_console_utf8(  "\U0001F525\U0001F363\U0001F431");       // 炎 寿司 猫 (utf-8)
    set_cursor_position(10,  2);
    write_console_utf16(L"\U0001F525\U0001F363\U0001F431");       // 炎 寿司 猫 (utf-16)
    set_cursor_position(10,  3);
    write_console_utf16(L"\xD83D\xDD25\xD83C\xDF63\xD83D\xDC31"); // 炎 寿司 猫 (utf-16 surrogate pair)

    set_cursor_position(30,  0);
    set_foreground_color_rgb24(255,   0,   0);
    set_background_color_rgb24(128, 128, 128);
    write_console_utf8("あいうえお");

    set_cursor_position( 0,  5);
    set_foreground_color_rgb24(192, 192, 192);
    set_background_color_rgb24(  0,   0,   0);
    set_bold(1);
    write_console_utf8("12345ABCDEあいうえお");
    set_bold(0);

    set_cursor_position( 0,  6);
    set_italic(1);
    write_console_utf8("12345ABCDEあいうえお");
    set_italic(0);

    set_cursor_position( 0,  7);
    set_underline(1);
    write_console_utf8("12345ABCDEあいうえお");
    set_underline(0);

    set_cursor_position( 0,  8);
    set_reverse(1);
    write_console_utf8("12345ABCDEあいうえお");
    set_reverse(0);

    get_screen_size(&width, &height);
    sprintf_s(str_buf, 100, "screen-width=%d",  width);
    set_cursor_position( 0, 10);
    write_console_utf8(str_buf);
    sprintf_s(str_buf, 100, "screen-height=%d", height);
    set_cursor_position( 0, 11);
    write_console_utf8(str_buf);

    set_cursor_position(10,  5);
    get_cursor_position(&x, &y);
    sprintf_s(str_buf, 100, "cursor-x=%d", x);
    set_cursor_position( 0, 12);
    write_console_utf8(str_buf);
    sprintf_s(str_buf, 100, "cursor-y=%d", y);
    set_cursor_position( 0, 13);
    write_console_utf8(str_buf);

    set_cursor_position( 0, 15);
    write_console_utf8("Hit any key. ");

    set_cursor_visible(0);
    set_cursor_visible(1);

    // key input
    getchar();

    // display test 2
    clear_screen();
    get_screen_size(&width, &height);
    for (i = 0; i < height; i++) {
        i2 = i % color_table_num;
        set_foreground_color_rgb24(color_table[i2][0], color_table[i2][1], color_table[i2][2]);
        set_cursor_position( 0,  i);
        write_console_utf8("1234567890あいうえおかきくけこさしすせそ");
        i3 = (i + 1) % color_table_num;
        set_foreground_color_rgb24(color_table[i3][0], color_table[i3][1], color_table[i3][2]);
        set_cursor_position(40,  i);
        write_console_utf8("1234567890あいうえおかきくけこさしすせそ");
    }
    set_cursor_visible(0);
    set_foreground_color_rgb24(192, 192, 192);
    set_background_color_rgb24(  0,   0,   0);

    // key input
    getchar();

    // end screen
    end_screen(1, con_input_mode_orig, con_output_mode_orig);

    return 0;
}
