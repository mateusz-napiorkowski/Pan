import pygame
import os
import random


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

display_width = 1000
display_height = 700

gameDisplay = pygame.display.set_mode((display_width, display_height))
pygame.display.set_caption('Pan')
background = pygame.image.load('background.jpg')
black = (0, 0, 0)
white = (255, 255, 255)

card_images = os.listdir('cards')
cards_number = 12
player_cards = random.sample(card_images, cards_number)
cards = pygame.sprite.Group()
cards_positions = [(x, 550) for x in range((1000-(30*(cards_number-1)+100))//2+50, 1000, 30)][:cards_number]
for i, card_name in enumerate(player_cards):
    cards.add(Card(f'cards/{card_name}', cards_positions[i][0], cards_positions[i][1]))
chosen_cards = [False] * cards_number
pygame.display.update()
while True:
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                pygame.quit()
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x_pos, mouse_y_pos = pygame.mouse.get_pos()
            chosen_card_index = None
            for card_index, card in enumerate(cards):
                if card.rect.collidepoint(mouse_x_pos, mouse_y_pos):
                    chosen_card_index = card_index
            if chosen_card_index is not None:
                cards.sprites()[chosen_card_index].update(cards_positions[chosen_card_index][0], 525)
                chosen_cards[chosen_card_index] = True
    pygame.display.flip()
    gameDisplay.blit(background, (0, 0))
    cards.draw(gameDisplay)
    clock.tick(60)
quit()