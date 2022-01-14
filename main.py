import pygame
import time


class Card(pygame.sprite.Sprite):
    def __init__(self, image_path, x_pos, y_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([100, 145])
        self.image = pygame.image.load(image_path)
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
card2 = Card('cards/9_of_clubs.png', 120, 120)

cards = pygame.sprite.Group()
cards.add(card)
cards.add(card2)
pygame.display.update()
while True:
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                pygame.quit()
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x_pos, mouse_y_pos = pygame.mouse.get_pos()
            for card in cards:
                if card.rect.collidepoint(mouse_x_pos, mouse_y_pos):
                    card.update(500, 500)
    pygame.display.flip()
    gameDisplay.blit(background, (0, 0))
    cards.draw(gameDisplay)
    clock.tick(60)
quit()