import pygame
import time

pygame.init()

display_width = 700
display_height = 700

gameDisplay = pygame.display.set_mode((display_width, display_height))
pygame.display.set_caption('Pan')

black = (0, 0, 0)
white = (255, 255, 255)

img = pygame.image.load('cards/9_of_diamonds.png')
img = pygame.transform.scale(img, (100, 100))

x = (display_width * 0.0)
y = (display_height * 0.0)

gameDisplay.fill(white)
gameDisplay.blit(img, (x, y))
pygame.display.update()
while True:
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                pygame.quit()
quit()