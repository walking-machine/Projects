import pygame as pg
import math
from copy import deepcopy


class Board:
    back = (0, 0, 0)
    grid = (0, 255, 0)
    text = (255, 255, 255)

    def __init__(self, data, screen):
        self.data = deepcopy(data)
        self.screen = screen
        self.width = math.floor(screen.get_size()[0]/9)
        self.font = pg.font.SysFont("monospace", math.floor(self.width*0.75))

    # Drawing the current state of the board
    def draw(self):

        # clear the screen
        self.screen.fill(self.back)

        # lines separating 3x3 blocks
        pg.draw.line(self.screen, self.grid, [0, self.width*3], [self.width*9, self.width*3], 3)
        pg.draw.line(self.screen, self.grid, [0, self.width*6], [self.width*9, self.width*6], 3)
        pg.draw.line(self.screen, self.grid, [self.width*3, 0], [self.width*3, self.width*9], 3)
        pg.draw.line(self.screen, self.grid, [self.width*6, 0], [self.width*6, self.width*9], 3)

        # draw rectangle and a number for each cell
        for i in range(9):
            for j in range(9):
                pg.draw.rect(self.screen, self.grid, [i*self.width, j*self.width, (i+1)*self.width, (j+1)*self.width], 1)

                # Do not draw text if the cell is empty
                if self.data[j][i] != 0:
                    # text rendering
                    text = str(self.data[j][i])
                    text_width = self.font.size(text)[0]
                    text_height = self.font.size(text)[1]
                    text = self.font.render(text, 1, self.text)
                    self.screen.blit(text, (i*self.width + (self.width-text_width)/2, j*self.width + (self.width -
                                                                                                      text_height)/2))

        pg.display.flip()  # updating the display
        pg.time.Clock().tick(10)  # setting FPS to 10


# Recurrent solving of the board
def solve(table: Board):
    pg.event.get()  # prevent pygame window from not responding
    for i in range(9):
        for j in range(9):
            if table.data[i][j] == 0:
                for n in range(1, 10):
                    if valid(table, i, j, n):
                        table.data[i][j] = n
                        table.draw()
                        if solve(table):
                            return True
                        table.data[i][j] = 0
                return False
    return True


# Bool determining if some number can be placed at the certain cell
def valid(table: Board, x: int, y: int, num: int):

    # top left corner of the 3x3 block
    x_start = math.floor(x/3)*3
    y_start = math.floor(y/3)*3
    # bottom right corner of the 3x3 block
    x_end = x_start+3
    y_end = y_start+3
    for i in range(x_start, x_end):
        for j in range(y_start, y_end):
            if table.data[i][j] == num:
                return False
    for i in range(9):
        if table.data[x][i] == num:
            return False
        if table.data[i][y] == num:
            return False
    return True

