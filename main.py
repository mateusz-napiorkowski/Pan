import pygame
import os
import random
import socket
import re
import select


class Card(pygame.sprite.Sprite):
    def __init__(self, image_path, x_pos, y_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([100, 145])
        self.image = pygame.image.load(image_path)
        self.rect = self.image.get_rect()
        self.rect.center = [x_pos, y_pos]

    def update(self, x_pos, y_pos):
        self.rect.center = [x_pos, y_pos]


class Button(pygame.sprite.Sprite):
    def __init__(self, image_path, x_pos, y_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([140, 40])
        self.image = pygame.image.load(image_path)
        self.rect = self.image.get_rect()
        self.rect.center = [x_pos, y_pos]

class Player_label(pygame.sprite.Sprite):
    def __init__(self, image_path, x_pos, y_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([400, 80])
        self.image = pygame.image.load(image_path)
        self.rect = self.image.get_rect()
        self.rect.center = [x_pos, y_pos]


host = '127.0.0.1'
port = 1100

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))
    #s.setblocking(0)
    #s.send(bytes(msg, 'utf-8'))


    pygame.init()
    clock = pygame.time.Clock()

    display_width = 1000
    display_height = 700

    gameDisplay = pygame.display.set_mode((display_width, display_height))
    pygame.display.set_caption('Pan')
    background = pygame.image.load('background.jpg')

    buttons = pygame.sprite.Group()
    buttons.add(Button('play_button.png', 400, 660))
    buttons.add(Button('draw_button.png', 600, 660))


    card_images = sorted(os.listdir('cards'), key=lambda x: int(re.match('(\d+).*', x).groups()[0]))
    card_indices = [i for i in range(24)]
    cards_number = 12
    data = s.recv(13)
    player_cards = [byte for byte in data]
    which_player = player_cards.pop()
    labels = pygame.sprite.Group()
    if which_player == 1:
        labels.add(Player_label('player1.png', 200, 660))
        labels.add(Player_label('player2.png', 200, 40))
    else:
        labels.add(Player_label('player1.png', 200, 40))
        labels.add(Player_label('player2.png', 200, 660))

    cards = pygame.sprite.Group()
    cards_positions = [(x, 550) for x in range((1000-(30*(cards_number-1)+100))//2+50, 1000, 30)][:cards_number]
    for i, card_name in enumerate(player_cards):
        cards.add(Card(f'cards/{card_name}.png', cards_positions[i][0], cards_positions[i][1]))
    chosen_cards = ['0'] * cards_number
    chosen_cards_to_send = ['0'] * 24
    while True:
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    s.close()
                    pygame.quit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x_pos, mouse_y_pos = pygame.mouse.get_pos()
                chosen_card_index = None
                for card_index, card in enumerate(cards):
                    if card.rect.collidepoint(mouse_x_pos, mouse_y_pos):
                        chosen_card_index = card_index
                if chosen_card_index is not None:
                    if chosen_cards[chosen_card_index] == '0':
                        cards.sprites()[chosen_card_index].update(cards_positions[chosen_card_index][0], 525)
                        chosen_cards[chosen_card_index] = '1'
                    else:
                        cards.sprites()[chosen_card_index].update(cards_positions[chosen_card_index][0], 550)
                        chosen_cards[chosen_card_index] = '0'
                for i, button in enumerate(buttons):
                    if button.rect.collidepoint(mouse_x_pos, mouse_y_pos):
                        if i == 0:
                            for j, player_card in enumerate(player_cards):
                                if chosen_cards[j] == '1':
                                    chosen_cards_to_send[player_card] = '1'
                            s.send(bytes(''.join(chosen_cards_to_send), 'utf-8'))
                            chosen_cards_to_send = ['0'] * 24
                            print('waiting for data')
                            data = s.recv(1)
                            ok = [byte for byte in data]
                            print(ok)
                            if chr(ok[0]) == '1':
                                print('Valid move')
                            else:
                                print('Invalid move')
        pygame.display.flip()
        gameDisplay.blit(background, (0, 0))
        ready = select.select([s], [], [], 0.01)
        if ready[0]:
            whose_turn = int(s.recv(1))
        if which_player == whose_turn:
            buttons.draw(gameDisplay)
        cards.draw(gameDisplay)
        labels.draw(gameDisplay)
        clock.tick(60)
