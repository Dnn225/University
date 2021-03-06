#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>

#include "resourceHandler.hpp"
#include "tile.hpp"


class Board
{
public:
	/*	Инициализация должна происходить единожды за всё время работы. */
	void init(ResourceHandler const &);

	/*	Доска сбрасывается для новой игры. */
	void reset();

	/*	Возвращает true, если на поле не осталось ходов
		после создания очередной плитки. */
	bool isGameOver() const;

	/*	Возвращает количество очков. */
	int score() const;

	/*	Если событие содержит нужную нам клавишу, то передаём управление в 
		функцию move. */
	void update(sf::Event const &);

	/*	Отрисовка доски и плиток на экране */
	void draw(sf::RenderWindow &) const;

private:
	/*	Добавляет плитку на доску в новой позиции.
		Значение плитки будет {2, 4, 8} с вероятностью соответственно {50%, 35%, 15%}
		(!) Требуется наличие хотя бы одной свободной позиции. */
	void createTile();

	/*	Производит сдвиг всех плиток в соответсвующем направлении. */
	void move(sf::Keyboard::Key);

	/*	Производит сдвиг всех плиток в массиве, сформированном 
		по следующему правилу: 
		... */
	void moveRowOnLeft(std::vector<Tile *> &);

	/*	Проверяет игру на проигрыш */
	void checkGameOver();

	/*	Возращает true, если доска заполнена */
	bool isFull() const;

	/*	Конвертирует всевозможные значения плиток (0, 2, 4,..) в 
		индексы спрайтов, соответсвующие им (0, 1, 2,..) */
	unsigned int ValueToSpriteIndex(int) const;

private:
	bool m_bBlocked;
	bool m_bGameIsOver;
	int m_score;

	sf::Sprite m_background;
	std::vector<sf::Sprite> m_sprites;
	std::vector<std::vector<Tile>> m_tiles;
};

#endif