import pygame
import time


class Card(pygame.sprite.Sprite):
    def __init__(self, imagepath, x_pos, y_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([100, 145])
        self.image = pygame.image.load(imagepath)
        self.rect = self.image.get_rect()
        self.rect.center = [x_pos, y_pos]
    def update(self, x_pos, y_pos):
        self.rect.center = [x_pos, y_pos]
pygame.init()
clock = pygame.time.Clock()

display_width = 700
display_height = 700

gameDisplay = pygame.display.set_mode((display_width, display_height))
pygame.display.set_caption('Pan')
background = pygame.image.load('background.jpg')
black = (0, 0, 0)
white = (255, 255, 255)


card = Card('cards/9_of_diamonds.png', 100, 100)

cards = pygame.sprite.Group()
cards.add(card)
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
        if event.type == pygame.MOUSEBUTTONDOWN:
            pos = pygame.mouse.get_pos()
            cards.update(500, 500)
            #clicked_sprites = [s for s in sprites if s.rect.collidepoint(pos)]
    pygame.display.flip()
    gameDisplay.blit(background, (0,0))
    cards.draw(gameDisplay)
    clock.tick(60)
quit()