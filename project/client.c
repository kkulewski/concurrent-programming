#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>

#define BOARD_SIZE 10 // plansza ma wymiary BOARD_SIZE x BOARD_SIZE pól
#define CELL_SIZE_PX 20 // rozmiar pola planszy w pikselach
#define BOARD_X 0 // odległość między planszą a lewym brzegiem okna
#define BOARD_Y 50 // odległość między planszą a górnym brzegiem okna
#define BOTTOM_MARGIN 50 // odległość między planszą a dolnym brzegiem okna
#define BOARD_SIZE_PX BOARD_SIZE * CELL_SIZE_PX // rozmiar planszy w pikselach

// zmienne globalne Xlib
Display *display;
Window window;
int screen;
GC gc;
XEvent event;

void draw_board(int board[BOARD_SIZE][BOARD_SIZE])
{
  Colormap colormap = DefaultColormap(display, screen);
  XColor color, exact_color;

  XAllocNamedColor(display, colormap, "black", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE + 1; i++)
  {
    // rysuje linie poziome
    XDrawLine(display, window, gc, BOARD_X, BOARD_Y + i * CELL_SIZE_PX, BOARD_X + BOARD_SIZE_PX, BOARD_Y + i * CELL_SIZE_PX);
  }


  XAllocNamedColor(display, colormap, "green", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE + 1; i++)
  {
    // rysuje linie pionowe
    XDrawLine(display, window, gc, BOARD_X + i * CELL_SIZE_PX, BOARD_Y, BOARD_X + i * CELL_SIZE_PX, BOARD_Y + BOARD_SIZE_PX);
  }

  XAllocNamedColor(display, colormap, "blue", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      if (board[i][j] == 1)
      {
        XFillRectangle(display, window, gc, BOARD_X + i * CELL_SIZE_PX, BOARD_Y + j * CELL_SIZE_PX, CELL_SIZE_PX, CELL_SIZE_PX);
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
  window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 300, 300, 1, BlackPixel(display, screen), WhitePixel(display, screen));
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

  /* add two taken points */
  board[2][3] = 1;
  board[7][1] = 1;

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
      printf("expose\n");
    }
    /* print mouse click location */
    if(event.type == ButtonPress)
    {
      printf("mouse X:%i Y:%i\n", event.xbutton.x, event.xbutton.y);
    }
    /* print key pressed */
    if (event.type == KeyPress)
    {
      printf("key pressed\n");
    }
    /* handle windows close event */
    if(event.type == ClientMessage)
    {
      printf("window close\n");
      break;
    }
  }

  /* destroy our window */
  XDestroyWindow(display, window);
  /* close connection to server */
  XCloseDisplay(display);
  return 0;
}
