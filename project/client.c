#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>

#define BOARD_SIZE 10
#define CELL_SIZE_PX 20
#define BOARD_SIZE_PX BOARD_SIZE * CELL_SIZE_PX
#define BOARD_X_MARGIN 20
#define BOARD_Y_MARGIN 20

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

coords_t get_cell_by_xy(int x, int y)
{
  int board = 0;
  int cell_x = (x - BOARD_X_MARGIN) / CELL_SIZE_PX;
  int cell_y = (y - BOARD_Y_MARGIN) / CELL_SIZE_PX;
  coords_t coords = { .board = board, .x = cell_x, .y = cell_y };
  return coords;
}

void draw_board(int board[BOARD_SIZE][BOARD_SIZE])
{
  Colormap colormap = DefaultColormap(display, screen);
  XColor color, exact_color;

  XAllocNamedColor(display, colormap, "blue", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE + 1; i++)
  {
    // horizontal lines
    XDrawLine(display, window, gc, BOARD_X_MARGIN, BOARD_Y_MARGIN + i * CELL_SIZE_PX, BOARD_X_MARGIN + BOARD_SIZE_PX, BOARD_Y_MARGIN + i * CELL_SIZE_PX);
  }


  XAllocNamedColor(display, colormap, "blue", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE + 1; i++)
  {
    // vertical lines
    XDrawLine(display, window, gc, BOARD_X_MARGIN + i * CELL_SIZE_PX, BOARD_Y_MARGIN, BOARD_X_MARGIN + i * CELL_SIZE_PX, BOARD_Y_MARGIN + BOARD_SIZE_PX);
  }

  XAllocNamedColor(display, colormap, "gray", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      if (board[i][j] == 1)
      {
        // filled rectangles
        XFillRectangle(display, window, gc, BOARD_X_MARGIN + i * CELL_SIZE_PX + 1, BOARD_Y_MARGIN + j * CELL_SIZE_PX + 1, CELL_SIZE_PX - 1, CELL_SIZE_PX - 1);
      }
    }
  }

  XFlush(display);
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
    BOARD_SIZE_PX + 2 * BOARD_X_MARGIN,
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

  /* create game board */
  int board[BOARD_SIZE][BOARD_SIZE];
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      board[i][j] = 0;
    }
  }

  /* add example filled fields */
  board[2][3] = 1;
  board[7][1] = 1;
  board[6][8] = 1;

  /* event loop */
  while(1)
  {
    draw_board(board);
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
      printf("cell X:%i Y:%i\n", selected_cell.x, selected_cell.y);
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
