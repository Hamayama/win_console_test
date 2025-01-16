
// win_console_input
// 2025-1-16 v1.01

#include "win_con_lib_1000.h"

int main(void)
{
    DWORD con_input_mode_orig;
    DWORD con_output_mode_orig;
    int input_loop;
    int ret_val;
    BOOL ret;
    WORD event_type;
    BOOL kdown;
    WORD repeat;
    WORD vkey;
    WORD vscan;
    WCHAR uchar;
    DWORD ctrl;
    SHORT mouse_x;
    SHORT mouse_y;
    DWORD mouse_button;
    DWORD mouse_ctrl;
    DWORD mouse_event_flag;
    int width;
    int height;

    // clear screen
    system("cls");

    // init screen
    init_screen(&con_input_mode_orig, &con_output_mode_orig);

    printf("Please try key and mouse input.\n");
    printf("( [q] key to quit )\n");

    // input loop
    input_loop = 1;
    ret_val = 0;
    while (input_loop) {

        // read console input
        ret = read_console_input(&event_type, &kdown, &repeat, &vkey, &vscan, &uchar, &ctrl,
                                 &mouse_x, &mouse_y, &mouse_button, &mouse_ctrl, &mouse_event_flag);
        if (ret == FALSE) {
            ret_val = 1;
            break;
        }
        if (event_type == 0) {
            Sleep(10);
            continue;
        }

        // process event
        switch (event_type) {
            case KEY_EVENT:
                // control keys
                //   Left Ctrl   : ctrl=0x8
                //   Right Ctrl  : ctrl=0x104
                //   Left Alt    : ctrl=0x2
                //   Right Alt   : ctrl=0x101
                //   Left Shift  : ctrl=0x10
                //   Right Shift : ctrl=0x10
                printf("key event : kdown=%d repeat=%d vkey=0x%x vscan=0x%x uchar=0x%x ctrl=0x%lx\n",
                        kdown, repeat, vkey, vscan, uchar, ctrl);

                // process key event
                if (!kdown) {
                    switch (vkey) {
                        // [q] key to quit
                        case 0x51:
                            input_loop = 0;
                            break;
                        // [l] key to clear screen
                        case 0x4c:
                            clear_screen();
                            break;
                    }
                }
                break;

            case MOUSE_EVENT:
                // mouse event
                //   Button            : evt=0
                //   Move              : evt=1
                //   Double Click      : evt=2
                //   Wheele Vertical   : evt=4
                //   Wheele Horizontal : evt=8
                // control keys
                //   Left Ctrl   : ctrl=0x8
                //   Right Ctrl  : ctrl=0x8
                //   Left Alt    : ctrl=0x2
                //   Right Alt   : ctrl=0x2
                //   Left Shift  : no data (text selection)
                //   Right Shift : no data (text selection)
                printf("mouse event : (%d,%d) btn=0x%lx ctrl=0x%lx evt=0x%lx\n",
                       mouse_x, mouse_y, mouse_button, mouse_ctrl, mouse_event_flag);
                break;

            case WINDOW_BUFFER_SIZE_EVENT:
                // get screen size
                ret = get_screen_size(&width, &height);
                if (ret == FALSE) {
                    input_loop = 0;
                    ret_val = 1;
                    break;
                }
                printf("window buffer size event : width=%d height=%d\n", width, height);
                break;
        }
    }

    // end screen
    end_screen(1, con_input_mode_orig, con_output_mode_orig);

    return ret_val;
}
