import pygame
import time


class Card(pygame.sprite.Sprite):
    def __init__(self, width, height, pos_x, pos_y, color):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([width, height]) # ([100,145])
        self.image.fill(color)
        self.rect = self.image.get_rect()
        self.rect.center = [pos_x, pos_y]

pygame.init()
clock = pygame.time.Clock()

display_width = 700
display_height = 700

gameDisplay = pygame.display.set_mode((display_width, display_height))
pygame.display.set_caption('Pan')
background = pygame.image.load('background.jpg')
black = (0, 0, 0)
white = (255, 255, 255)


card = Card(100, 145, 100, 100, white)

cards = pygame.sprite.Group()
cards.add(card)

#img = pygame.image.load('cards/9_of_diamonds.png')
#img = pygame.transform.scale(img, (100, 145))

#x = (display_width * 0.0)
#y = (display_height * 0.0)

#gameDisplay.fill(white)
#gameDisplay.blit(img, (x, y))
pygame.display.update()
sprites = pygame.sprite.Group()
print(sprites)
while True:
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                print(sprites)
                pygame.quit()
        if event.type == pygame.MOUSEBUTTONUP:
            pos = pygame.mouse.get_pos()
            #clicked_sprites = [s for s in sprites if s.rect.collidepoint(pos)]
    pygame.display.flip()
    gameDisplay.blit(background, (0,0))
    cards.draw(gameDisplay)
    clock.tick(60)
quit()