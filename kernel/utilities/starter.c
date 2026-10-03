#include "starter.h"
#include "drivers/vga.h"
#include "drivers/pit.h"
#include "drivers/speaker.h"
#include "utilities/iolayer.h"
#include "common_headers/types.h"
#include <stdbool.h>

#define GLYPH_W 5
#define GLYPH_H 5
#define GLYPH_GAP 1
#define SPACE_W 4
#define BLOCK_CHAR ((char)0xDB)
#define SHADE_CHAR ((char)0xB1)
#define RULE_CHAR ((char)0xCD)
#define COLOR_BLACK 0
#define COLOR_CYAN 3
#define COLOR_MAGENTA 5
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_LIGHT_RED 12
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

typedef struct
{
    char letter;
    const char *rows[GLYPH_H];
} glyph_t;

static const glyph_t font[] =
{
    { 'F', { "#####", "#....", "####.", "#....", "#...." } },
    { 'O', { ".###.", "#...#", "#...#", "#...#", ".###." } },
    { 'R', { "####.", "#...#", "####.", "#..#.", "#...#" } },
    { 'T', { "#####", "..#..", "..#..", "..#..", "..#.." } },
    { 'H', { "#...#", "#...#", "#####", "#...#", "#...#" } },
    { 'S', { ".####", "#....", ".###.", "....#", "####." } }
};

static const uint8_t row_colors[GLYPH_H] =
{
    COLOR_YELLOW,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_MAGENTA
};

static const uint32_t intro_notes[] = { 523, 587, 659, 784, 880, 1047, 1319 };
static const uint32_t finale_notes[] = { 784, 1047, 1319, 1568 };

#define INTRO_NOTE_COUNT ((int)(sizeof(intro_notes) / sizeof(intro_notes[0])))
#define FINALE_NOTE_COUNT ((int)(sizeof(finale_notes) / sizeof(finale_notes[0])))

static int text_length(const char *text)
{
    int len = 0;

    while (text[len] != '\0')
    {
        len++;
    }

    return len;
}

static void clear_screen(void)
{
    uint8_t color = vga_entry_color(COLOR_LIGHT_GREY, COLOR_BLACK);

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vga_put_char(' ', color, x, y);
        }
    }
}

static const glyph_t *find_glyph(char c)
{
    for (unsigned int i = 0; i < sizeof(font) / sizeof(font[0]); i++)
    {
        if (font[i].letter == c)
        {
            return &font[i];
        }
    }

    return 0;
}

static int char_x(const char *title, int index, int start_x)
{
    int x = start_x;

    for (int i = 0; i < index; i++)
    {
        if (title[i] == ' ')
        {
            x += SPACE_W;
        }
        else
        {
            x += GLYPH_W + GLYPH_GAP;
        }
    }

    return x;
}

static void draw_glyph(const glyph_t *glyph, int x, int y, bool flash)
{
    uint8_t shadow = vga_entry_color(COLOR_DARK_GREY, COLOR_BLACK);

    for (int row = 0; row < GLYPH_H; row++)
    {
        for (int col = 0; col < GLYPH_W; col++)
        {
            if (glyph->rows[row][col] == '#')
            {
                vga_put_char(SHADE_CHAR, shadow, x + col + 1, y + row + 1);
            }
        }
    }

    for (int row = 0; row < GLYPH_H; row++)
    {
        uint8_t fg = flash ? COLOR_WHITE : row_colors[row];
        uint8_t color = vga_entry_color(fg, COLOR_BLACK);

        for (int col = 0; col < GLYPH_W; col++)
        {
            if (glyph->rows[row][col] == '#')
            {
                vga_put_char(BLOCK_CHAR, color, x + col, y + row);
            }
        }
    }
}

static void draw_title_char(const char *title, int index, int start_x, int y, bool flash)
{
    const glyph_t *glyph = find_glyph(title[index]);

    if (glyph != 0)
    {
        draw_glyph(glyph, char_x(title, index, start_x), y, flash);
    }
}

static void draw_title_all(const char *title, int start_x, int y, bool flash)
{
    for (int i = 0; title[i] != '\0'; i++)
    {
        draw_title_char(title, i, start_x, y, flash);
    }
}

static void draw_rules(int top_y, int bottom_y)
{
    int half = VGA_WIDTH / 2;
    uint8_t top_color = vga_entry_color(COLOR_MAGENTA, COLOR_BLACK);
    uint8_t bottom_color = vga_entry_color(COLOR_CYAN, COLOR_BLACK);

    for (int step = 0; step < half; step++)
    {
        vga_put_char(RULE_CHAR, top_color, half - 1 - step, top_y);
        vga_put_char(RULE_CHAR, top_color, half + step, top_y);
        vga_put_char(RULE_CHAR, bottom_color, half - 1 - step, bottom_y);
        vga_put_char(RULE_CHAR, bottom_color, half + step, bottom_y);
        pit_sleep(10);
    }
}

static void type_text(const char *text, int y, uint8_t color)
{
    int len = text_length(text);
    int x = (VGA_WIDTH - len) / 2;

    for (int i = 0; i < len; i++)
    {
        vga_put_char(text[i], color, x + i, y);
        pit_sleep(35);
    }
}

void starter_run(void)
{
    const char *title = "FORTH OS";
    const char *subtitle = "32-BIT PROTECTED MODE  -  FORTH INSIDE";
    int len = text_length(title);
    int width = char_x(title, len, 0) - GLYPH_GAP;
    int start_x = (VGA_WIDTH - width) / 2;
    int y = (VGA_HEIGHT - GLYPH_H) / 2 - 1;
    int note = 0;

    clear_screen();
    draw_rules(y - 2, y + GLYPH_H + 2);
    pit_sleep(150);

    for (int i = 0; i < len; i++)
    {
        if (title[i] == ' ')
        {
            pit_sleep(120);
            continue;
        }

        draw_title_char(title, i, start_x, y, false);
        speaker_beep(intro_notes[note % INTRO_NOTE_COUNT], 110);
        pit_sleep(40);
        note++;
    }

    pit_sleep(100);

    for (int k = 0; k < FINALE_NOTE_COUNT; k++)
    {
        draw_title_all(title, start_x, y, (k % 2) == 0);
        speaker_beep(finale_notes[k], 90);
    }

    draw_title_all(title, start_x, y, false);
    speaker_beep(2093, 400);

    type_text(subtitle, y + GLYPH_H + 4, vga_entry_color(COLOR_LIGHT_GREY, COLOR_BLACK));

    pit_sleep(1200);
    clear_screen();
    set_line(0);
}