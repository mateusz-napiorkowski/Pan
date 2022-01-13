import pygame
import time

pygame.init()

display_width = 800
display_height = 600

gameDisplay = pygame.display.set_mode((display_width, display_height))
pygame.display.set_caption('Pan')

black = (0, 0, 0)
white = (255, 255, 255)

img = pygame.image.load('9.png')


x = (display_width * 0.5)
y = (display_height * 0.5)

gameDisplay.fill(white)
gameDisplay.blit(img, (x, y))
pygame.display.update()
time.sleep(2)
pygame.quit()
quit()