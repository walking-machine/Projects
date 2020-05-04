import pygame as pg
from board import Board
from board import solve

pg.init()
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
size = (450, 450)  # screen size

# Opening a window
screen = pg.display.set_mode(size)
pg.display.set_caption("My First Game")

pg.font.init()  # enabling text in pygame

# creating a 9x9 array of zeros
data = [[0 for i in range(9)] for j in range(9)]

difficulty = input("Enter puzzle number (1 to 4): ")
while True:
    if difficulty.isdigit():
        if int(difficulty) == 1:
            f = open("boards/simple.txt")
            break
        if int(difficulty) == 2:
            f = open("boards/easy.txt")
            break
        if int(difficulty) == 3:
            f = open("boards/intermed.txt")
            break
        if int(difficulty) == 4:
            f = open("boards/expert.txt")
            break
    difficulty = input("Enter a proper puzzle number (1 to 4): ")

# opening an input file
f = open("boards/expert.txt")

# reading data
for i in range(9):
    for j in range(9):
        k = f.read(1)
        if k.isdigit():
            data[i][j] = int(k)

f.close()  # closing a file

# waiting for ENTER key to start solving with visible board
table = Board(data, screen)
table.draw()
wait = True
while wait:
    event = pg.event.wait()
    if event.type == pg.KEYUP and event.key == pg.K_RETURN:
        wait = False

solve(table)  # solving

# wait until escape key pressed or game window closed
wait = True
while wait:
    event = pg.event.wait()
    if event.type == pg.QUIT:
        wait = False
    if event.type == pg.KEYUP and event.key == pg.K_ESCAPE:
        wait = False
pg.quit()
