#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define GAME_SHIPS 6
#define BOARD_SIZE 8

#define CELL_SIZE_PX 25
#define BOARD_SIZE_PX BOARD_SIZE * CELL_SIZE_PX
#define BOARD_X_MARGIN 40
#define BOARD_Y_MARGIN 40

#define FIELD_EMPTY 0
#define FIELD_SHIP 1
#define FIELD_HIT 2
#define FIELD_MISS 3

#define BOARD_NONE 0
#define BOARD_PLAYER 1
#define BOARD_ENEMY 2

typedef enum { false, true } bool;

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
Colormap colormap;
XColor color, exact_color;

// game global variables
int player_board[BOARD_SIZE][BOARD_SIZE];
int enemy_board[BOARD_SIZE][BOARD_SIZE];
char* status_message;

/* initialize boards */
void init_boards()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      player_board[i][j] = FIELD_EMPTY;
      enemy_board[i][j] = FIELD_EMPTY;
    }
  }
  /* add example enemy ships */
  enemy_board[4][4] = FIELD_SHIP;
  enemy_board[1][2] = FIELD_SHIP;
  enemy_board[2][2] = FIELD_SHIP;
  enemy_board[4][6] = FIELD_SHIP;
  enemy_board[5][6] = FIELD_SHIP;
  enemy_board[6][6] = FIELD_SHIP;
}

/* draw player and enemy board grid */
void draw_grid()
{
  XAllocNamedColor(display, colormap, "blue", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);

  int x1, y1, x2, y2;
  int offset_x = BOARD_SIZE_PX + BOARD_X_MARGIN;
  for (int i = 0; i < BOARD_SIZE + 1; i++)
  {
    // horizontal lines
    x1 = BOARD_X_MARGIN;
    y1 = BOARD_Y_MARGIN + i * CELL_SIZE_PX;
    x2 = BOARD_X_MARGIN + BOARD_SIZE_PX;
    y2 = BOARD_Y_MARGIN + i * CELL_SIZE_PX;
    XDrawLine(display, window, gc, x1, y1, x2, y2);
    XDrawLine(display, window, gc, x1 + offset_x, y1, x2 + offset_x, y2);

    // vertical lines
    x1 = BOARD_X_MARGIN + i * CELL_SIZE_PX;
    y1 = BOARD_Y_MARGIN;
    x2 = BOARD_X_MARGIN + i * CELL_SIZE_PX;
    y2 = BOARD_Y_MARGIN + BOARD_SIZE_PX;
    XDrawLine(display, window, gc, x1, y1, x2, y2);
    XDrawLine(display, window, gc, x1 + offset_x, y1, x2 + offset_x, y2);
  }
}

/* draw player fields */
void draw_player_board()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      switch (player_board[i][j])
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
      int x1 = BOARD_X_MARGIN + i * CELL_SIZE_PX + 1;
      int y1 = BOARD_Y_MARGIN + j * CELL_SIZE_PX + 1;
      int width = CELL_SIZE_PX - 1;
      int height = CELL_SIZE_PX - 1;
      XFillRectangle(display, window, gc, x1, y1, width, height);
    }
  }
}

/* draw enemy board fields */
void draw_enemy_board()
{
  int offset_x = BOARD_SIZE_PX + BOARD_X_MARGIN;
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      switch (enemy_board[i][j])
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

      /* draw rectangle with selected color */
      XSetForeground(display, gc, color.pixel);
      int x1 = BOARD_X_MARGIN + i * CELL_SIZE_PX + 1;
      int y1 = BOARD_Y_MARGIN + j * CELL_SIZE_PX + 1;
      int width = CELL_SIZE_PX - 1;
      int height = CELL_SIZE_PX - 1;
      XFillRectangle(display, window, gc, x1 + offset_x, y1, width, height);
    }
  }
}

/* draw titles above both boards */
void draw_titles()
{
  XAllocNamedColor(display, colormap, "black", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);

  char* title = "Your board:";
  int x = BOARD_X_MARGIN;
  int y = BOARD_Y_MARGIN - 5;
  XDrawString(display, window, gc, x, y, title, strlen(title));

  title = "Enemy board:";
  x = BOARD_X_MARGIN * 2 + BOARD_SIZE_PX;
  y = BOARD_Y_MARGIN - 5;
  XDrawString(display, window, gc, x, y, title, strlen(title));
}

/* draw game status message (bottom of the window) */
void draw_status()
{
  /* draw rectangle to hide previous message */
  XAllocNamedColor(display, colormap, "green", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  int x = 0;
  int y = BOARD_SIZE_PX + BOARD_Y_MARGIN + 15;
  int width = (BOARD_SIZE_PX + BOARD_X_MARGIN) * 2 + BOARD_X_MARGIN;
  int height = BOARD_Y_MARGIN;
  XFillRectangle(display, window, gc, x, y, width, height);

  /* draw new message */
  XAllocNamedColor(display, colormap, "black", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  x = BOARD_X_MARGIN;
  y = BOARD_SIZE_PX + BOARD_Y_MARGIN * 2 - 8;
  XDrawString(display, window, gc, x, y, status_message, strlen(status_message));
}

/* draw current game state to display */
void draw_game_state()
{
  draw_grid();
  draw_player_board();
  draw_enemy_board();
  draw_titles();
  draw_status();
  XFlush(display);
}

/* use XY to determine selected cell */
coords_t get_cell_by_xy(int x, int y)
{
  int board = BOARD_NONE;
  int cell_x = 0;
  int cell_y = 0;

  bool player_x = x >= BOARD_X_MARGIN && x <= BOARD_SIZE_PX + BOARD_X_MARGIN;
  bool player_y = y >= BOARD_Y_MARGIN && y <= BOARD_Y_MARGIN + BOARD_SIZE_PX;
  if (player_x == true && player_y == true)
  {
    board = BOARD_PLAYER;
  }

  bool enemy_x = (x >= BOARD_SIZE_PX + 2*BOARD_X_MARGIN) && (x <= 2*BOARD_SIZE_PX + 2*BOARD_X_MARGIN);
  bool enemy_y = (y >= BOARD_Y_MARGIN) && (y <= BOARD_Y_MARGIN + BOARD_SIZE_PX);
  if (enemy_x == true && enemy_y == true)
  {
    board = BOARD_ENEMY;
    // substract px offset - this way enemy board can be handled same as player board
    int offset = (BOARD_SIZE_PX + BOARD_X_MARGIN);
    x -= offset;
  }

  cell_x = (x - BOARD_X_MARGIN) / CELL_SIZE_PX;
  cell_y = (y - BOARD_Y_MARGIN) / CELL_SIZE_PX;
  coords_t coords = { .board = board, .x = cell_x, .y = cell_y };
  return coords;
}

/* attempt to shoot at given field */
int shoot_at(coords_t selected_cell)
{
  printf("TARGET: B:%i X:%i Y:%i\n", selected_cell.board, selected_cell.x, selected_cell.y);

  if (selected_cell.board == BOARD_NONE)
  {
    printf("RESULT: none - no board\n");
    status_message = "Illegal move - no board";
    return BOARD_NONE;
  }

  if (selected_cell.board == BOARD_PLAYER)
  {
    printf("RESULT: none - own board\n");
    status_message = "Illegal move - own board";
    return BOARD_PLAYER;
  }

  int field = enemy_board[selected_cell.x][selected_cell.y];
  if (selected_cell.board == BOARD_ENEMY)
  {
    if (field == FIELD_EMPTY || field == FIELD_MISS)
    {
      enemy_board[selected_cell.x][selected_cell.y] = FIELD_MISS;
      printf("RESULT: miss\n");
      status_message = "Missed!";
    }
    if (field == FIELD_SHIP || field == FIELD_HIT)
    {
      enemy_board[selected_cell.x][selected_cell.y] = FIELD_HIT;
      printf("RESULT: hit\n");
      status_message = "Enemy hit!";
    }
  }
}

/* attempt to add ship on given field */
int add_ship(coords_t selected_cell)
{
  printf("TARGET: B:%i X:%i Y:%i\n", selected_cell.board, selected_cell.x, selected_cell.y);

  if (selected_cell.board == BOARD_NONE)
  {
    printf("RESULT: none - no board\n");
    status_message = "Illegal move - no board";
    return 0;
  }

  if (selected_cell.board == BOARD_ENEMY)
  {
    printf("RESULT: none - enemy board\n");
    status_message = "Illegal move - enemy board";
    return 0;
  }

  int field = player_board[selected_cell.x][selected_cell.y];
  if (selected_cell.board == BOARD_PLAYER)
  {
    if (field == FIELD_EMPTY)
    {
      player_board[selected_cell.x][selected_cell.y] = FIELD_SHIP;
      printf("RESULT: ship added\n");
      status_message = "Ship added!";
      return 1;
    }
    if (field == FIELD_SHIP)
    {
      printf("RESULT: field occupied\n");
      status_message = "Field is already occupied!";
      return 0;
    }
  }
}

void init_display()
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
  window = XCreateSimpleWindow
  (
    display,
    RootWindow(display, screen),
    0,
    0,
    2 * (BOARD_SIZE_PX + BOARD_X_MARGIN) + BOARD_X_MARGIN,
    BOARD_SIZE_PX + 2 * BOARD_Y_MARGIN,
    1,
    BlackPixel(display, screen),
    WhitePixel(display, screen)
  );
  /* process window close event through event handler so XNextEvent does Not fail */
  Atom delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(display, window, &delete_window, 1);
  /* grab mouse pointer location */
  XGrabPointer(display, window, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  /* select kind of events we are interested in */
  XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);
  /* map (show) the window */
  XMapWindow(display, window);
  /* get display colormap */
  colormap = DefaultColormap(display, screen);
}

/* dispose display */
void dispose_display()
{
  /* destroy our window */
  XDestroyWindow(display, window);
  /* close connection to server */
  XCloseDisplay(display);
}

/* in this phase, player sets up ships on his board */
int setup_loop()
{
  status_message = "Setup your ships.";
  int ships_to_add = GAME_SHIPS;
  while (ships_to_add > 0)
  {
    draw_game_state();
    XNextEvent(display, &event);
    printf("Event:: ");
    if(event.type == Expose)
    {
      printf("exposed\n");
    }
    if(event.type == ButtonPress)
    {
      printf("mouse pressed X:%i Y:%i\n", event.xbutton.x, event.xbutton.y);
      int added = add_ship(get_cell_by_xy(event.xbutton.x, event.xbutton.y));
      ships_to_add -= added;
    }
    if(event.type == ClientMessage)
    {
      printf("window closed\n");
      return -1;
    }
  }
  return 0;
}

/* in this phase, player selects fields on enemy board to shoot at */
void game_loop()
{
  status_message = "Game started. Select field to shoot at.";
  while(1)
  {
    draw_game_state();
    XNextEvent(display, &event);
    printf("Event:: ");
    if(event.type == Expose)
    {
      printf("exposed\n");
    }
    if(event.type == ButtonPress)
    {
      printf("mouse pressed X:%i Y:%i\n", event.xbutton.x, event.xbutton.y);
      shoot_at(get_cell_by_xy(event.xbutton.x, event.xbutton.y));
    }
    if(event.type == ClientMessage)
    {
      printf("window closed\n");
      return;
    }
  }
}

int main()
{
  init_display();
  init_boards();

  int state = setup_loop();
  if (state == -1)
  {
    dispose_display();
    return -1;
  }

  game_loop();
  dispose_display();
  return 0;
}
