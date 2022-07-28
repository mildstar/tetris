#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "winlib_db.h"
#include "kbhit.h"
#include "blocks.h"
#define MODENUM 3
#define Blocksize 20
#define ESC 27
#define SCOREFILE "score.txt"

// NOTE Block Initialization
struct Block
{
  int block[BlockRotate][BlockSIZE][BlockSIZE];
  int type;
  int rot;
  int curX, curY;
};

struct Block curblock;
struct Block nextblock;
void InitBlock(struct Block *block)
{
  srand(time(NULL));
  int random = 0;
  random = rand() % 7;
  block->type = random;
  block->rot = 0;
  block->curX = 4;
  block->curY = 0;
  int x, y, r;
  for (r = 0; r < 4; r++)
  {
    for (y = 0; y < 4; y++)
    {
      for (x = 0; x < 4; x++)
      {
        block->block[r][y][x] = Blocks[block->type][r][y][x];
      }
    }
  }
}

int score = 0;
int scorelist[12] = {
    0,
};
char score_str[10] = "0";

int mode = 0;
int pause = 0;
int cursor_main = 0;
int cursor_pause = 0;

int waittime = 10000;

// NOTE Function Declaration
Bool IsCrash(struct Block *block);
Bool IsGameOver();
void InitBoard();
void fixBlock(struct Block *block);
void deleteLine();
void drawBoard();
void Update(struct Block *curblock);
void draw();
void readFile();

// NOTE Block Control Function
void *func(void *arg)
{
  while (1)
  {
    switch (mode)
    {
    case 0: //! mode 0 (Lobby)
      draw();
      break;

    case 1: //! mode 1 (In Game)
      score = 0;
      InitBlock(&curblock);
      InitBlock(&nextblock);
      InitBoard();
      sprintf(score_str, "%d", score);
      while (mode == 1)
      {
        if (pause == 0)
        {
          curblock.curY += 1;
          if (IsCrash(&curblock) == True)
          {
            curblock.curY -= 1;
            fixBlock(&curblock);
          }
          deleteLine();
          Update(&curblock);

          draw();
          // usleep(waittime);
        }
        else
        {
          draw();
        }
      }
      break;

    case 2: //! mode 2 (Leaderboard)
      draw();
      break;

    case 4: //! mode 4 (Game Over)
      draw();
      break;
    }
  }
}

// NOTE Main : KeyBoard Input Function
int main()
{
  srand((unsigned int)time(NULL));
  initwin();
  int c;

  pthread_t pthread;
  pthread_create(&pthread, NULL, &func, NULL);
  readFile();
  while (1)
  {
    switch (mode)
    {
    case 0: //! mode 0 (Lobby)
      if (kbhit())
      {
        c = getchar();
        switch (c)
        {
        case 'w':
          cursor_main = (cursor_main - 1 + MODENUM) % MODENUM;
          break;

        case 's':
          cursor_main = (cursor_main + 1 + MODENUM) % MODENUM;
          break;

        case '\n':
          mode = cursor_main + 1;
          break;
        }
      }
      break;

    case 1: //! mode 1 (In Game)
      if (kbhit())
      {
        c = getchar();
        switch (c)
        {
        case 'w':
          if (pause == 0)
          {
            curblock.rot = (curblock.rot + 1 + 4) % 4;
            if (IsCrash(&curblock) == True)
            {
              curblock.rot = (curblock.rot - 1 + 4) % 4;
            }
          }
          else
          {
            cursor_pause = (cursor_pause - 1 + 3) % 3;
          }
          break;

        case 'a':
          if (pause == 0)
          {
            curblock.curX -= 1;
            if (IsCrash(&curblock) == True)
            {
              curblock.curX += 1;
            }
          }
          break;

        case 's':
          if (pause == 0)
          {
            curblock.rot = (curblock.rot - 1 + 4) % 4;
            if (IsCrash(&curblock) == True)
            {
              curblock.rot = (curblock.rot + 1 + 4) % 4;
            }
          }
          else
          {
            cursor_pause = (cursor_pause + 1 + 3) % 3;
          }
          break;

        case 'd':
          if (pause == 0)
          {
            curblock.curX += 1;
            if (IsCrash(&curblock) == True)
            {
              curblock.curX -= 1;
            }
          }
          break;

        case ' ':
          while (1)
          {
            curblock.curY += 1;
            if (IsCrash(&curblock) == True)
            {
              curblock.curY -= 1;
              break;
            }
          }
          break;

        case ESC:
          if (pause == 0)
          {
            pause = 1;
          }
          else
          {
            pause = 0;
          }
          break;

        case '\n':
          if (pause == 1)
          {
            if (cursor_pause == 0)
            {
              pause = 0;
            }
            else if (cursor_pause == 1)
            {
              mode = 0;
              pause = 0;
            }
            else if (cursor_pause == 2)
            {
              mode = 3;
              pause = 0;
            }
            cursor_pause = 0;
          }
          break;
        }
      }
      break;

    case 2: //! mode 2 (Leaderboard)
      if (kbhit())
      {
        c = getchar();
        if (c == '\n')
        {
          mode = 0;
        }
      }
      break;

    case 3: //! mode 3 (Exit Game)
      return 0;

    case 4: //! mode 4 (Game Over)
      if (kbhit())
      {
        c = getchar();
        if (c == '\n')
        {
          mode = 0;
        }
      }
      break;
    }
  }
  return 0;
}

// NOTE Draw Function
void draw()
{
  g_clear();
  g_rgb(4864, 13056, 19456);
  g_fill(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
  g_rgb(65535, 65535, 65535);
  int i;
  switch (mode)
  {
  case 0:
    g_rgb(64768, 24320, 0);
    g_fill(DEFAULT_WIDTH / 2 - 65, DEFAULT_HEIGHT / 3 - 65 - 3, 130, 130);
    g_rgb(65535, 65535, 65535);
    text(DEFAULT_WIDTH / 2 - 32, DEFAULT_HEIGHT / 3, "T E T R I S");
    text(DEFAULT_WIDTH / 2 - 65, DEFAULT_HEIGHT / 2 + 10, "P L A Y");
    text(DEFAULT_WIDTH / 2 - 65, DEFAULT_HEIGHT / 2 + 60, "L E A D E R B O A R D");
    text(DEFAULT_WIDTH / 2 - 65, DEFAULT_HEIGHT / 2 + 110, "E X I T");
    text(DEFAULT_WIDTH / 2 - 115, DEFAULT_HEIGHT / 2 + 190, "P R E S S  E N T E R  T O  S E L E C T");
    g_fill(DEFAULT_WIDTH / 2 - 85, DEFAULT_HEIGHT / 2 + 50 * cursor_main + 3, 5, 5);
    break;

  case 1:
    if (pause == 0)
    {
      drawBoard();
      text(15, 25, "SCORE :");
      text(63, 25, score_str);
    }
    else
    {
      g_rgb(65535, 65535, 65535);
      text(DEFAULT_WIDTH / 2 - 30, DEFAULT_HEIGHT / 3, "P A U S E");

      text(DEFAULT_WIDTH / 2 - 30, DEFAULT_HEIGHT / 2, "R E S U M E");
      text(DEFAULT_WIDTH / 2 - 30, DEFAULT_HEIGHT / 2 + 50, "L O B B Y");
      text(DEFAULT_WIDTH / 2 - 30, DEFAULT_HEIGHT / 2 + 100, "E X I T");
      g_fill(DEFAULT_WIDTH / 2 - 50, DEFAULT_HEIGHT / 2 + 50 * cursor_pause - 7, 5, 5);
    }
    break;

  case 2:
  {
    text(DEFAULT_WIDTH / 2 - 65, DEFAULT_HEIGHT / 3, "L E A D E R B O A R D");
    int i;
    char rank[10];
    for (i = 0; i < 10; i++)
    {
      sprintf(score_str, "%d", scorelist[i]);
      sprintf(rank, "%d", i + 1);
      text(DEFAULT_WIDTH / 2 - 30, DEFAULT_HEIGHT / 3 + 60 + 30 * i, "# ");
      text(DEFAULT_WIDTH / 2 - 20, DEFAULT_HEIGHT / 3 + 60 + 30 * i, rank);
      text(DEFAULT_WIDTH / 2 + 10, DEFAULT_HEIGHT / 3 + 60 + 30 * i, score_str);
    }
    break;
  }

  case 4:
    text(DEFAULT_WIDTH / 2 - 50, DEFAULT_HEIGHT / 3, "G A M E  O V E R");
    text(DEFAULT_WIDTH / 2 - 50, DEFAULT_HEIGHT / 3 + 40, "S C O R E :");
    text(DEFAULT_WIDTH / 2 + 20, DEFAULT_HEIGHT / 3 + 40, score_str);
    break;
  }
  display_update();
}

// NOTE Control Functions

Bool IsCrash(struct Block *block)
{
  int i, j;
  int x, y;
  for (y = 0; y < BlockSIZE; y++)
  {
    for (x = 0; x < BlockSIZE; x++)
    {
      if (block->block[block->rot][y][x] == 2)
      {
        if (Board[block->curY + y][block->curX + x] == 1 || Board[block->curY + y][block->curX + x] == 3)
        {
          return True;
        }
      }
    }
  }
  return False;
}

void InitBoard()
{
  int y, x;
  for (y = 0; y < BoardHEIGHT; y++)
  {
    for (x = 1; x < BoardWIDTH + 1; x++)
    {
      FixedBoard[y][x] = 0;
      Board[y][x] = 0;
    }
  }
}

void fixBlock(struct Block *block)
{
  int i, j;
  int x, y;
  for (y = 0; y < BlockSIZE; y++)
  {
    for (x = 0; x < BlockSIZE; x++)
    {
      if (block->block[block->rot][y][x] == 2)
      {
        FixedBoard[block->curY + y][block->curX + x] = 3;
      }
    }
  }
  *block = nextblock;
  InitBlock(&nextblock);
}

void deleteLine()
{
  int x, y;
  int cnt = 0;
  int line_del = 0;
  for (y = BoardHEIGHT - 1; y > 0; y--)
  {
    cnt = 0;
    for (x = 1; x < BoardWIDTH + 1; x++)
    {
      if (FixedBoard[y][x] == 0 || FixedBoard[y][x] == 2)
      {
        cnt += 1;
      }
    }
    if (cnt == 0)
    {
      score += 10;
      sprintf(score_str, "%d", score);
      for (x = 1; x < BoardWIDTH + 1; x++)
      {
        FixedBoard[y][x] = 0;
      }
      int x2, y2;
      for (y2 = y; y2 > 0; y2--)
      {
        for (x2 = 1; x2 < BoardWIDTH + 1; x2++)
        {
          FixedBoard[y2][x2] = FixedBoard[y2 - 1][x2];
        }
      }
    }
  }
}

void drawBoard()
{
  int x, y;
  int realX, realY;
  g_fill(Blocksize * (-5) + DEFAULT_WIDTH / 2 - 2, Blocksize * (-10) + DEFAULT_HEIGHT / 2 - 2, Blocksize * (BoardWIDTH) + 2, Blocksize * (-6) + DEFAULT_HEIGHT / 2 + 2);
  g_rgb(62976, 62976, 59648);
  g_fill(Blocksize * (-5) + DEFAULT_WIDTH / 2 - 2, Blocksize * (-6) + DEFAULT_HEIGHT / 2 - 2, Blocksize * (BoardWIDTH) + 2, Blocksize * (BoardHEIGHT - 4) + 2);
  for (y = 0; y < BoardHEIGHT; y++)
  {
    for (x = 1; x < BoardWIDTH + 1; x++)
    {
      realX = Blocksize * (x - 6) + DEFAULT_WIDTH / 2;
      realY = Blocksize * (y - 10) + DEFAULT_HEIGHT / 2;

      switch (Board[y][x])
      {
      case 2:
        g_rgb(64768, 24320, 0);
        g_fill(realX, realY, 18, 18);
        g_rgb(65535, 65535, 65535);
        break;

      case 3:
        g_rgb(24320, 0, 64768);
        g_fill(realX, realY, 18, 18);
        g_rgb(65535, 65535, 65535);
        break;
      }
    }
  }

  for (y = 0; y < BlockSIZE; y++)
  {
    for (x = 0; x < BlockSIZE; x++)
    {
      realX = Blocksize * (x + 7) + DEFAULT_WIDTH / 2;
      realY = Blocksize * (y - 10) + DEFAULT_HEIGHT / 2;
      if (nextblock.block[nextblock.rot][y][x] == 2)
      {
        g_rgb(64768, 24320, 0);
        g_fill(realX, realY, 18, 18);
        g_rgb(65535, 65535, 65535);
      }
    }
  }
  text(Blocksize * 7 + DEFAULT_WIDTH / 2, Blocksize * (-5) + DEFAULT_HEIGHT / 2, "next block");
}

void Update(struct Block *curblock)
{
  int x, y;
  int ny, nx;
  int realX, realY;
  for (y = 0; y < BoardHEIGHT; y++)
  {
    for (x = 1; x < BoardWIDTH + 1; x++)
    {
      Board[y][x] = FixedBoard[y][x];
    }
  }
  for (y = curblock->curY; y < curblock->curY + 4; y++)
  {
    for (x = curblock->curX; x < curblock->curX + 4; x++)
    {
      if (IsGameOver() == False)
      {
        Board[y][x] += curblock->block[curblock->rot][y - curblock->curY][x - curblock->curX];
      }
      else
      {
        scorelist[11] = score;
        score = 0;
        readFile("score.txt");
        mode = 4;
      }
    }
  }
}

Bool IsGameOver()
{
  int x;
  for (x = 1; x < BoardWIDTH + 1; x++)
  {
    if (FixedBoard[3][x] != 0)
    {
      return True;
    }
  }
  return False;
}

void readFile()
{
  FILE *fp;
  char buf[100];
  fp = fopen(SCOREFILE, "r+");
  if (fp == NULL)
    printf("Cannot open file: %s\n", SCOREFILE);

  int i, j, temp;
  for (i = 0; i < 10; i++)
  {
    if (fgets(buf, sizeof(buf), fp) != NULL)
    {
      scorelist[i] = atoi(buf);
    }
  }
  for (i = 0; i < 11; i++)
  {
    for (j = 0; j < 11 - i; j++)
    {
      if (scorelist[j] < scorelist[j + 1])
      {
        temp = scorelist[j];
        scorelist[j] = scorelist[j + 1];
        scorelist[j + 1] = temp;
      }
    }
  }
  rewind(fp);
  for (i = 0; i < 10; i++)
  {
    fprintf(fp, "%d\n", scorelist[i]);
  }
  fclose(fp);
}