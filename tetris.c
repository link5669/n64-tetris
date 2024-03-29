#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <libdragon.h>

#define BOARD_WIDTH 6
#define BOARD_HEIGHT 20

typedef struct Coordinate
{
  int x, y;
} Coordinate;

char board[BOARD_WIDTH][BOARD_HEIGHT];

const char pieces[7][4][4] = {
    {{'C', 'C', ' ', ' '},
     {'C', 'C', ' ', ' '},
     {' ', ' ', ' ', ' '},
     {' ', ' ', ' ', ' '}},
    {{'C', ' ', ' ', ' '},
     {'C', ' ', ' ', ' '},
     {'C', ' ', ' ', ' '},
     {'C', ' ', ' ', ' '}},
    {{' ', 'C', 'C', 'C'},
     {'C', 'C', 'C', ' '},
     {' ', ' ', ' ', ' '},
     {' ', ' ', ' ', ' '}},
    {{'C', 'C', 'C', ' '},
     {' ', 'C', 'C', 'C'},
     {' ', ' ', ' ', ' '},
     {' ', ' ', ' ', ' '}},
    {{'C', ' ', ' ', ' '},
     {'C', ' ', ' ', ' '},
     {'C', 'C', ' ', ' '},
     {' ', ' ', ' ', ' '}},
    {{' ', 'C', ' ', ' '},
     {' ', 'C', ' ', ' '},
     {'C', 'C', ' ', ' '},
     {' ', ' ', ' ', ' '}},
    {{'C', 'C', 'C', ' '},
     {' ', 'C', ' ', ' '},
     {' ', ' ', ' ', ' '},
     {' ', ' ', ' ', ' '}},
};

void draw_new_piece(const char (*piece)[4])
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      board[i + 1][j] = piece[i][j];
    }
  }
}

void mark_current_as_set()
{
  for (int i = 0; i < BOARD_WIDTH; i++)
  {
    for (int j = 0; j < BOARD_HEIGHT; j++)
    {
      if (board[i][j] == 'C')
      {
        board[i][j] = 'X';
      }
    }
  }
}

void translate_left()
{
  for (int i = 1; i < BOARD_WIDTH; i++)
  {
    for (int j = 0; j < BOARD_HEIGHT; j++)
    {
      if (board[i][j] == 'C')
      {
        board[i - 1][j] = 'C';
        board[i][j] = ' ';
      }
    }
  }
}

void translate_right()
{
  for (int i = BOARD_WIDTH - 2; i >= 0; i--)
  {
    for (int j = BOARD_HEIGHT - 1; j >= 0; j--)
    {
      if (board[i][j] == 'C')
      {
        board[i + 1][j] = 'C';
        board[i][j] = ' ';
      }
    }
  }
}

void draw_board()
{
  surface_t *disp = display_get();
  for (int i = 0; i < BOARD_WIDTH; i++)
  {
    for (int j = 0; j < BOARD_HEIGHT; j++)
    {
      if (board[i][j] == 'C')
        graphics_draw_box(disp, 10 + (i * 10), 10 + (j * 10), 5, 5, graphics_make_color(50, 200, 50, 255));
      if (board[i][j] == ' ')
        graphics_draw_box(disp, 10 + (i * 10), 10 + (j * 10), 5, 5, graphics_make_color(0, 0, 0, 255));
      if (board[i][j] == 'X')
        graphics_draw_box(disp, 10 + (i * 10), 10 + (j * 10), 5, 5, graphics_make_color(100, 100, 100, 255));
    }
  }
  display_show(disp);
}

int main(void)
{
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 1, GAMMA_NONE, FILTERS_RESAMPLE);
  rdp_init();
  controller_init();
  timer_init();

  surface_t *disp;

  while (1)
  {
    disp = display_get();

    graphics_fill_screen(disp, graphics_make_color(0, 0, 0, 255));
    display_show(disp);

    int r = rand() % 7;

    draw_new_piece(pieces[r]);

    Coordinate bottommost_coordinate = {.x = 0, .y = 0};

    for (int i = 0; i < BOARD_WIDTH; i++)
    {
      for (int j = 0; j < BOARD_HEIGHT; j++)
      {
        if (board[i][j] == 'C')
          if (bottommost_coordinate.y < j)
          {
            bottommost_coordinate.y = j;
            bottommost_coordinate.x = i;
          }
      }
    }

    disp = display_get();

    graphics_fill_screen(disp, graphics_make_color(0, 0, 0, 255));

    display_show(disp);

    int vertical_offset = 0;

    while (board[bottommost_coordinate.x][bottommost_coordinate.y + 1] != 'X' && bottommost_coordinate.y != BOARD_HEIGHT - 1)
    {

      for (int i = 3; i >= 0; i--)
      {
        for (int j = 3; j >= 0; j--)
        {
          if (board[i + 1][j + vertical_offset] == 'C')
          {
            board[i + 1][j + vertical_offset + 1] = 'C';
            board[i + 1][j + vertical_offset] = ' ';
          }
        }
      }
      vertical_offset++;

      draw_board();

      for (int i = 0; i < 100; i++)
      {
        controller_scan();

        struct controller_data keys = get_keys_down();

        if (keys.c[0].right)
        {
          translate_right();
          draw_board();
        }

        if (keys.c[0].left)
        {
          translate_left();
          draw_board();
        }

        wait_ms(1);
      }

      bottommost_coordinate.x = 0;
      bottommost_coordinate.y = 0;

      for (int i = 0; i < BOARD_WIDTH; i++)
      {
        for (int j = 0; j < BOARD_HEIGHT; j++)
        {
          if (board[i][j] == 'C')
            if (bottommost_coordinate.y < j)
            {
              bottommost_coordinate.y = j;
              bottommost_coordinate.x = i;
            }
        }
      }
    }
    mark_current_as_set();
  }
}
