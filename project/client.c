#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>

#define BOARD_SIZE 10
#define CELL_SIZE_PX 20
#define BOARD_SIZE_PX BOARD_SIZE * CELL_SIZE_PX
#define BOARD_X_MARGIN 20
#define BOARD_Y_MARGIN 20

#define FIELD_EMPTY 0
#define FIELD_SHIP 1
#define FIELD_HIT 2
#define FIELD_MISS 3
#define BOARD_NONE 0
#define BOARD_PLAYER 1
#define BOARD_ENEMY 2

typedef struct coords_st {
  int board;
  int x;
  int y;
} coords_t;

// xlib global variables
Display *display;
Window window;
int screen;
GC gc;
XEvent event;

// game global variables
int p_board[BOARD_SIZE][BOARD_SIZE];
int e_board[BOARD_SIZE][BOARD_SIZE];

void init_boards()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      p_board[i][j] = FIELD_EMPTY;
      e_board[i][j] = FIELD_EMPTY;
    }
  }

  /* add example player ships */
  p_board[2][3] = FIELD_SHIP;
  p_board[3][3] = FIELD_SHIP;
  p_board[7][1] = FIELD_SHIP;
  p_board[8][1] = FIELD_SHIP;
  p_board[9][1] = FIELD_SHIP;
  /* add example enemy ships */
  e_board[4][6] = FIELD_SHIP;
  p_board[5][6] = FIELD_SHIP;
  p_board[6][6] = FIELD_SHIP;
}

void draw_p_board()
{
    Colormap colormap = DefaultColormap(display, screen);
    XColor color, exact_color;

    XAllocNamedColor(display, colormap, "blue", &color, &exact_color);
    XSetForeground(display, gc, color.pixel);
    for (int i = 0; i < BOARD_SIZE + 1; i++)
    {
      // horizontal lines
      XDrawLine(display, window, gc, BOARD_X_MARGIN, BOARD_Y_MARGIN + i * CELL_SIZE_PX, BOARD_X_MARGIN + BOARD_SIZE_PX, BOARD_Y_MARGIN + i * CELL_SIZE_PX);
      // vertical lines
      XDrawLine(display, window, gc, BOARD_X_MARGIN + i * CELL_SIZE_PX, BOARD_Y_MARGIN, BOARD_X_MARGIN + i * CELL_SIZE_PX, BOARD_Y_MARGIN + BOARD_SIZE_PX);
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
      for (int j = 0; j < BOARD_SIZE; j++)
      {
        switch (p_board[i][j])
        {
          case FIELD_EMPTY:
              XAllocNamedColor(display, colormap, "white", &color, &exact_color);
              break;
          case FIELD_SHIP:
              XAllocNamedColor(display, colormap, "green", &color, &exact_color);
              break;
          case FIELD_MISS:
              XAllocNamedColor(display, colormap, "gray", &color, &exact_color);
              break;
          case FIELD_HIT:
              XAllocNamedColor(display, colormap, "red", &color, &exact_color);
              break;
        }

        XSetForeground(display, gc, color.pixel);
        XFillRectangle(display, window, gc, BOARD_X_MARGIN + i * CELL_SIZE_PX + 1, BOARD_Y_MARGIN + j * CELL_SIZE_PX + 1, CELL_SIZE_PX - 1, CELL_SIZE_PX - 1);
      }
    }
}

void draw_e_board()
{
  Colormap colormap = DefaultColormap(display, screen);
  XColor color, exact_color;

  XAllocNamedColor(display, colormap, "blue", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE + 1; i++)
  {
    // horizontal lines
    XDrawLine(display, window, gc, BOARD_X_MARGIN, BOARD_Y_MARGIN + i * CELL_SIZE_PX, BOARD_X_MARGIN + BOARD_SIZE_PX, BOARD_Y_MARGIN + i * CELL_SIZE_PX);
    // vertical lines
    XDrawLine(display, window, gc, BOARD_X_MARGIN + i * CELL_SIZE_PX, BOARD_Y_MARGIN, BOARD_X_MARGIN + i * CELL_SIZE_PX, BOARD_Y_MARGIN + BOARD_SIZE_PX);
  }

  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      switch (e_board[i][j])
      {
        case FIELD_EMPTY:
            XAllocNamedColor(display, colormap, "white", &color, &exact_color);
            break;
        case FIELD_SHIP:
            XAllocNamedColor(display, colormap, "white", &color, &exact_color);
            break;
        case FIELD_MISS:
            XAllocNamedColor(display, colormap, "gray", &color, &exact_color);
            break;
        case FIELD_HIT:
            XAllocNamedColor(display, colormap, "red", &color, &exact_color);
            break;
      }

      XSetForeground(display, gc, color.pixel);
      XFillRectangle(display, window, gc, BOARD_X_MARGIN + i * CELL_SIZE_PX + 1, BOARD_Y_MARGIN + j * CELL_SIZE_PX + 1, CELL_SIZE_PX - 1, CELL_SIZE_PX - 1);
    }
  }
}

void draw_boards()
{
  draw_p_board();
  draw_e_board();
  XFlush(display);
}

coords_t get_cell_by_xy(int x, int y)
{
  int board = 0;
  int cell_x = 0;
  int cell_y = 0;

  if (x >= BOARD_X_MARGIN && x <= BOARD_SIZE + BOARD_X_MARGIN)
  {
    board = BOARD_PLAYER;
  }

  if (x >= BOARD_SIZE + 2 * BOARD_X_MARGIN && x <= 2 * BOARD_SIZE + 2 * BOARD_X_MARGIN)
  {
    board = BOARD_ENEMY;
    x -= (BOARD_SIZE + BOARD_X_MARGIN);
  }

  cell_x = (x - BOARD_X_MARGIN) / CELL_SIZE_PX;
  cell_y = (y - BOARD_Y_MARGIN) / CELL_SIZE_PX;
  coords_t coords = { .board = board, .x = cell_x, .y = cell_y };
  return coords;
}

int make_move(coords_t selected_cell)
{

}

int main()
{
  /* open connection with the server */
  display = XOpenDisplay(NULL);
  if(display == NULL)
  {
    printf("Cannot open display\n");
    exit(1);
  }

  /* set screen */
  screen = DefaultScreen(display);
  /* set GC */
  gc = DefaultGC(display, screen);
  /* create window */
  window = XCreateSimpleWindow(
    display,
    RootWindow(display, screen),
    0,
    0,
    2 * (BOARD_SIZE_PX + 2 * BOARD_X_MARGIN),
    BOARD_SIZE_PX + 2 * BOARD_Y_MARGIN,
    1,
    BlackPixel(display, screen),
    WhitePixel(display, screen)
  );
  /* Process Window Close Event through event handler so XNextEvent does Not fail */
  Atom delWindow = XInternAtom(display, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(display, window, &delWindow, 1);
  /* grab mouse pointer location */
  XGrabPointer(display, window, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  /* select kind of events we are interested in */
  XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);
  /* map (show) the window */
  XMapWindow(display, window);

  /* create game boards */
  init_boards();

  /* event loop */
  while(1)
  {
    draw_boards();
    XNextEvent(display, &event);
    printf("Event:: ");

    /* draw or redraw the window */
    if(event.type == Expose)
    {
      //XFillRectangle(d, w, DefaultGC(d, s), 20, 20, 10, 10);
      printf("exposed\n");
    }
    /* print mouse click location */
    if(event.type == ButtonPress)
    {
      printf("mouse pressed X:%i Y:%i\n", event.xbutton.x, event.xbutton.y);
      coords_t selected_cell = get_cell_by_xy(event.xbutton.x, event.xbutton.y);
      printf("cell B:%i X:%i Y:%i\n", selected_cell.board, selected_cell.x, selected_cell.y);

      int field = p_board[selected_cell.x][selected_cell.y];
      if (field == FIELD_EMPTY || field == FIELD_MISS)
      {
        p_board[selected_cell.x][selected_cell.y] = FIELD_MISS;
      }
      if (field == FIELD_SHIP || field == FIELD_HIT)
      {
        p_board[selected_cell.x][selected_cell.y] = FIELD_HIT;
      }
    }
    /* print key pressed */
    if (event.type == KeyPress)
    {
      printf("key pressed\n");
    }
    /* handle windows close event */
    if(event.type == ClientMessage)
    {
      printf("window closed\n");
      break;
    }
  }

  /* destroy our window */
  XDestroyWindow(display, window);
  /* close connection to server */
  XCloseDisplay(display);
  return 0;
}
