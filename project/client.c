#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/shm.h>
#include<signal.h>
#include<unistd.h>

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

#define SHARED_KEY_1 9123
#define SHARED_KEY_2 9124
#define SHARED_KEY_3 9125

typedef enum { false, true } bool;

typedef struct coords_st {
  int board;
  int x;
  int y;
} coords_t;

struct timeval tv;

// xlib global variables
Display *display;
Window window;
int screen;
GC gc;
XEvent event;
Colormap colormap;
XColor color, exact_color;
int x11_file_descriptor;

// game global variables
char* status_message;
int player_id;
int enemy_id;
int hits;

int** player_board;
int** enemy_board;
int* player_turn;
int shm_board1_id;
int shm_board2_id;
int shm_player_turn_id;

int init_shared_state()
{
  /* first player to create shared_memory_1 becomes player 1 */
  if ((shm_board1_id = shmget(SHARED_KEY_1, 1024, 0666 | IPC_CREAT | IPC_EXCL)) != -1)
  {
    player_id = 1;
    enemy_id = 2;
    shm_board2_id = shmget(SHARED_KEY_2, 1024, 0666 | IPC_CREAT);
    shm_player_turn_id = shmget(SHARED_KEY_3, 1024, 0666 | IPC_CREAT);
    printf("Player ID: 1\n");
  }
  else
  {
    player_id = 2;
    enemy_id = 1;
    shm_board1_id = shmget(SHARED_KEY_1, 1024, 0666 | IPC_CREAT);
    shm_board2_id = shmget(SHARED_KEY_2, 1024, 0666 | IPC_CREAT);
    shm_player_turn_id = shmget(SHARED_KEY_3, 1024, 0666 | IPC_CREAT);
    printf("Player ID: 2\n");
  }

  /* get shared memory */
  int* shm_board_1 = shmat(shm_board1_id, 0, 0);
  int* shm_board_2 = shmat(shm_board2_id, 0, 0);
  player_turn = shmat(shm_player_turn_id, 0, 0);

  /* allocate space for array of pointers */
  player_board = malloc(BOARD_SIZE * sizeof(player_board[0]));
  enemy_board = malloc(BOARD_SIZE * sizeof(enemy_board[0]));

  /* make each pointer in board array point to shared memory */
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if (player_id == 1)
    {
      player_board[i] = shm_board_1 + i * BOARD_SIZE;
      enemy_board[i] = shm_board_2 + i * BOARD_SIZE;
    }
    if (player_id == 2)
    {
      player_board[i] = shm_board_2 + i * BOARD_SIZE;
      enemy_board[i] = shm_board_1 + i * BOARD_SIZE;
    }
  }

  /* player 1 is responsible for initialization */
  if (player_id == 1)
  {
    for (int i = 0; i < BOARD_SIZE; i++)
    {
      for (int j = 0; j < BOARD_SIZE; j++)
      {
        player_board[i][j] = FIELD_EMPTY;
        enemy_board[i][j] = FIELD_EMPTY;
      }
    }
    *player_turn = player_id;
  }
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
    status_message = "Illegal move - no board. Try again.";
    return BOARD_NONE;
  }

  if (selected_cell.board == BOARD_PLAYER)
  {
    printf("RESULT: none - own board\n");
    status_message = "Illegal move - own board. Try again.";
    return BOARD_PLAYER;
  }

  int field = enemy_board[selected_cell.x][selected_cell.y];
  if (selected_cell.board == BOARD_ENEMY)
  {
    if (field == FIELD_MISS)
    {
      enemy_board[selected_cell.x][selected_cell.y] = FIELD_MISS;
      printf("RESULT: duplicate miss\n");
      status_message = "Shot lost! Wait for enemy turn...";
    }

    if (field == FIELD_EMPTY)
    {
      enemy_board[selected_cell.x][selected_cell.y] = FIELD_MISS;
      printf("RESULT: miss\n");
      status_message = "Missed! Wait for enemy turn...";
    }

    if (field == FIELD_HIT)
    {
      printf("RESULT: duplicate hit\n");
      status_message = "Shot lost! Wait for enemy turn...";
    }

    if (field == FIELD_SHIP)
    {
      enemy_board[selected_cell.x][selected_cell.y] = FIELD_HIT;
      hits++;
      printf("RESULT: hit\n");
      status_message = "Hit! Wait for enemy turn...";
    }

    return BOARD_ENEMY;
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
  XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
  /* map (show) the window */
  XMapWindow(display, window);
  /* get display colormap */
  colormap = DefaultColormap(display, screen);
  /* display file descriptor */
  x11_file_descriptor = ConnectionNumber(display);
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
    if(event.type == Expose)
    {
      printf("Event:: exposed\n");
    }
    if(event.type == ButtonPress)
    {
      printf("Event:: mouse pressed X:%i Y:%i\n", event.xbutton.x, event.xbutton.y);
      int added = add_ship(get_cell_by_xy(event.xbutton.x, event.xbutton.y));
      ships_to_add -= added;
    }
    if(event.type == ClientMessage)
    {
      printf("Event:: window closed\n");
      *player_turn = -enemy_id;
      return -1;
    }
  }
  return 0;
}

/* in this phase, player selects fields on enemy board to shoot at */
void game_loop()
{
  fd_set in_fds;
  status_message = "Game started. Select field to shoot at.";
  while(*player_turn > 0)
  {
    // Create a file description set containing x11_fd
    FD_ZERO(&in_fds);
    FD_SET(x11_file_descriptor, &in_fds);

    // Set our timer
    tv.tv_usec = 0;
    tv.tv_sec = 1;

    // Wait for X Event or a Timer
    int num_ready_fds = select(x11_file_descriptor + 1, &in_fds, NULL, NULL, &tv);
    if (num_ready_fds == 0)
    {
        // Handle timer here
        if (*player_turn == player_id)
        {
          status_message = "Your turn.";
          draw_game_state();
        }
    }

    // Handle XEvents and flush the input
    while(XPending(display))
    {
      XNextEvent(display, &event);
      if(event.type == Expose)
      {
        printf("Event:: exposed\n");
      }
      if(event.type == ButtonPress)
      {
        printf("Event:: mouse pressed X:%i Y:%i\n", event.xbutton.x, event.xbutton.y);
        if (*player_turn == player_id)
        {
          int shoot_result = shoot_at(get_cell_by_xy(event.xbutton.x, event.xbutton.y));
          if (shoot_result == BOARD_ENEMY)
          {
            // shoot_result contains "hit" when same ship is hit twice. TODO: add additional IF in shot_at
            if (hits >= GAME_SHIPS)
            {
              *player_turn = -player_id;
            }
            else
            {
              *player_turn = enemy_id;
            }
          }
          draw_game_state();
        }
      }
      if(event.type == ClientMessage)
      {
        printf("Event:: window closed\n");
        *player_turn = -enemy_id;
        return;
      }
      XFlush(display);
    }
  }

  if (*player_turn == -player_id)
  {
    status_message = "You have won!";
  }
  if (*player_turn == -enemy_id)
  {
    status_message = "You have lost!";
  }
  draw_game_state();
  XNextEvent(display, &event);
}

void remove_shared_state(int signal)
{
  *player_turn = -enemy_id;
  shmctl(shm_board1_id, IPC_RMID, 0);
  shmctl(shm_board2_id, IPC_RMID, 0);
  shmctl(shm_player_turn_id, IPC_RMID, 0);
  exit(0);
}

int main()
{
  signal(SIGINT, remove_shared_state);

  init_display();
  init_shared_state();

  int state = setup_loop();
  if (state == -1)
  {
    dispose_display();
    return -1;
  }
  *player_turn = enemy_id;
  status_message = "Now wait for another player...";
  draw_game_state();
  while (*player_turn != player_id) { usleep(100 * 1000); }
  game_loop();
  dispose_display();
  remove_shared_state(0);
  return 0;
}
