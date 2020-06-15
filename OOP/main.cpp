// main.cpp : �� ���Ͽ��� 'main' �Լ��� ���Ե˴ϴ�. �ű⼭ ���α׷� ������ ���۵ǰ� ����˴ϴ�.
//

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <ctime>
#include <cstdlib> // include malloc,


// commit practice
// Ŀ�� ����

class Screen;
class GameObjectManager;
class GameObject; // forward declaration
class Enemy; // forward declaration
class BlinkableEnemy;
class Bullet;
class Player;

class Screen {
	int		len;
	char*	canvas;

public:
	Screen(int maxCount = 80) 
		: len(maxCount+1), canvas{ (char*) malloc(sizeof(char)*(maxCount+1))}
	{
	}

	// �Ҹ���, destructor
	~Screen()
	{
		free((void *)canvas);
		canvas = nullptr;
		len = 0;
	}

	int length() { return len-1; }

	void clear()
	{
		memset(canvas, ' ', len-1);
		canvas[len-1] = '\0';
	}

	void render()
	{
		printf("%s\r", canvas);
	}

	void draw(const char* shape, int pos)
	{
		strncpy(&canvas[pos], shape, strlen(shape));
	}

};

class GameObjectManager {
	GameObject** gos;
	int          capacity;

public:
	GameObjectManager(int capacity = 20) 
		: capacity{ capacity }, gos{ (GameObject**)malloc(sizeof(GameObject*)*capacity) }
	{
		for (int i = 0; i < capacity; i++)
			gos[i] = (GameObject*)nullptr;
	}
	~GameObjectManager() {
	
		for (int i = 0; i < capacity; i++)
		{
			if (gos[i] == nullptr) continue;
			delete gos[i];
			gos[i] = (GameObject*)nullptr;
		}
		free(gos);
		gos = (GameObject**)nullptr;
		capacity = 0;
	}

	void add(GameObject* obj)
	{
		if (obj == nullptr) return;
		// obj != nullptr
		for (int i = 0; i < capacity; i++)
		{
			if (gos[i] != nullptr) continue;
			// gos[i] == nullptr
			gos[i] = obj;
			return;
		}
		// i == capacity
		gos = (GameObject**)realloc(gos, sizeof(GameObject*) * 2 * capacity);
		for (int i = capacity; i < 2 * capacity; i++)
		{
			gos[i] = nullptr;
		}
		gos[capacity] = obj;
		capacity *= 2;
	}

	void remove(GameObject* obj)
	{
		if (obj == nullptr) return;
		for (int i = 0; i < capacity; i++)
		{
			if (gos[i] == obj) {
				gos[i] = nullptr;
				return;
			}
		}
	}

	GameObject** getGameObjects() { return gos; }
	int getCapacity() { return capacity; }
};

class GameObject {
	int		pos;
	char	shape[100]; // 0 ... 99
	static  GameObjectManager gameObjectManager;
	Screen& screen;

public:
	GameObject(Screen& screen, int pos, const char* shape)
		: screen(screen), pos(pos)
	{
		setShape(shape);
		gameObjectManager.add(this);
	}

	virtual ~GameObject() {} // ���� �Ҹ��� �Լ�

	//getter ����
	int getPos() const { return pos; }
	const char* getShape() const { return shape; }
	Screen& getScreen() const { return screen;  }

	static GameObject** getGameObjects() { return gameObjectManager.getGameObjects(); }
	static int getMaxGameObjects() { return gameObjectManager.getCapacity(); }
	
	//setter ����
	void setPos(int pos) { this->pos = pos;  }
	void setShape(const char* shape) 
	{
		int len = strlen(shape); // len >= 0
		if (len >= 100) {
			// 0 ... 98
			// 99���� �ι���
			strncpy(this->shape, shape, 100 - 1);
			this->shape[100 - 1] = '\0';
		}
		else
			strcpy(this->shape, shape);
	}


	bool isInside() const
	{
		return pos <= (screen.length() - strlen(shape)) && pos >= 0;
	}

	void moveRight() {
		pos++;
	}

	void moveLeft()
	{
		pos--;
	}

	virtual void process_input(int key) {}

	virtual void update() {}

	virtual void draw()
	{
		if (isInside() == false) return;
		screen.draw(shape, pos);
	}
};

class Enemy : public GameObject {
	
public:
	Enemy(Screen& screen, const char* shape)
		: GameObject(screen, rand() % (screen.length() - (int)strlen(shape)), shape )
	{}
	~Enemy() {}

	// overriding : ������
	void process_input(int key)
	{
		switch (key) {
		case 'w': moveRight(); break;
		case 's': moveLeft(); break;
		}
	}

	// overriding
	void update();	

	virtual void OnDamage() {}
};

class BlinkableEnemy : public Enemy {
	bool isBlinking;
	int count;

public:
	BlinkableEnemy(Screen& screen, const char* shape)
		: Enemy(screen, shape), isBlinking(false), count(0)
	{}

	void setBlinking() { 
		isBlinking = true;
		count = 10;
	}

	// overriding : ������
	void process_input(int key)
	{
		switch (key) {
		case 'j': moveLeft(); break;
		case 'k': moveRight(); break;
		}
	}

	// overriding
	void update()
	{
		Enemy::update();
		if (isBlinking == true) {
			count--;
			if (count == 0) {
				isBlinking = false;
			}
		}
	}

	void OnDamage()
	{
		setBlinking();
	}

	// overriding
	void draw()
	{
		if (isBlinking == false) {
			GameObject::draw();
			return;
		}
		if (count % 2 == 0) GameObject::draw();
	}

};

class Bullet : public GameObject {
	bool	isFired;
	int		direction;

public:
	Bullet(Screen& screen, const char* shape = "")
		: GameObject(screen, -1, shape), isFired(false), direction(0)
	{}

	~Bullet() {}

	bool checkFire()
	{
		return isFired == true;
	}
	void setFire()
	{
		isFired = true;
	}
	void resetFire()
	{
		isFired = false;
	}

	void makeDirectionLeft() { direction = 1; }
	void makeDirectionRight() { direction = 0; }
	bool isDirectionRight() { return direction == 0; }

	//overriding
	void update()
	{
		if (checkFire() == false) return;

		if (isInside() == false) {
			resetFire();
			return;
		}

		if (isDirectionRight())
			moveRight();
		else moveLeft();
	}

	//overriding
	void draw()
	{
		if (checkFire() == false) return;
		GameObject::draw();
	}
}; // ����ü Bullet ����

class Player : public GameObject {
	
	Bullet* findUnusedBullet()
	{
		GameObject** gos = getGameObjects();
		int maxGameObjects = getMaxGameObjects();
		for (int i = 0; i < maxGameObjects; i++)
		{
			GameObject* obj = gos[i];
			if (obj == nullptr) continue;

			// obj != nullptr
			Bullet* bullet = dynamic_cast<Bullet*>(obj);
			if (bullet == nullptr) continue;

			// bullet != nullptr
			if (bullet->checkFire() == true) continue;

			// bullet != nullptr && bullet->checkFire() == false
			return bullet;
		}
		return nullptr;
	}

	void fire()
	{
		Bullet* bullet = findUnusedBullet();
		if (bullet == nullptr) {
			bullet = new Bullet{ getScreen() } ;
		}
		// bullet != nullptr
		int pos = getPos();

		// bullet != nullptr && bullet->checkFire() == false
		Enemy* enemy = nullptr;
		Enemy* closest = nullptr;
		int closestDistance = getScreen().length();

		// find the closest enemy from game objects array.
		GameObject** gos = getGameObjects();
		int maxGameObjects = getMaxGameObjects();
		for (int i = 0; i < maxGameObjects; i++)
		{
			GameObject* obj = gos[i];
			if (obj == nullptr) continue;
			enemy = dynamic_cast<Enemy*>(obj);
			if (enemy == nullptr) continue;
			int dist = abs(enemy->getPos() - pos);
			if (closest == nullptr || (dist < closestDistance) ) {
				closest = enemy;
				closestDistance = dist;
			}
		}

		int enemy_pos = -1;
		if (closest == nullptr) {
			bool isLeft = rand() % 2 ? false : true;
			enemy_pos = getScreen().length() - 1;
			if (isLeft == true) enemy_pos = 0;
		}
		else {
			enemy_pos = closest->getPos();
		}
		bullet->setFire();
		
		bullet->setPos(pos);
		bullet->setShape("<--");
		bullet->makeDirectionLeft();
		if (pos < enemy_pos) {
			bullet->setPos(bullet->getPos() + (int)strlen(getShape()) - 1);
			bullet->setShape("-->");
			bullet->makeDirectionRight();
		}
	}

public:
	// constructor ������
	Player(Screen& screen, const char* shape)
		: GameObject(screen, rand() % (screen.length() - strlen(shape)), shape)
	{}

	~Player() {}

	// overriding
	void process_input(int key)
	{
		Bullet* bullet = nullptr;

		switch (key) {
		case 'a': moveLeft(); break;
		case 'd': moveRight(); break;
		case ' ':
			fire();
			break;
		}
	}
};

GameObjectManager GameObject::gameObjectManager{1};

void Enemy::update()
{
	if (isInside() == false) return;

	GameObject** gos = getGameObjects();
	int maxGameObjects = getMaxGameObjects();
	int pos = getPos();
	for (int i = 0; i < maxGameObjects; ++i)
	{
		GameObject* obj = gos[i];
		if (obj == nullptr) continue;
		Bullet* bullet = dynamic_cast<Bullet*>(obj);
		if (bullet == nullptr) continue;
		if (bullet->checkFire() == false) continue;
		if ((bullet->isDirectionRight() == true && bullet->getPos() + strlen(bullet->getShape()) == pos)
			|| (bullet->isDirectionRight() == false && pos + strlen(getShape()) == bullet->getPos())) {
			OnDamage();
			bullet->resetFire();
		}
	}
}

int main()
{
	Screen screen{ 80 };

	new BlinkableEnemy{ screen, "(*_*)" };
	new Enemy{ screen, "(+_+)" };
	new Player{ screen, "(o_o)" };
	
	bool requestExit = false;
	while (requestExit == false)		
	{
		screen.clear();
		GameObject** gos = GameObject::getGameObjects();
		int capacity = GameObject::getMaxGameObjects();

		// update game objects (player, enemy ...)
		// gos, maxGameObjects
		for (int i = 0; i < capacity; i++)
		{	
			GameObject* obj = gos[i];
			if (obj == nullptr) continue;			

			// obj != nullptr
			// search player
			Player* player = dynamic_cast<Player *>(obj); // dynamically downcast			
			if (player != nullptr) {
				// if player exists, check whether it is inside screen. otherwise, exit.
				if (player->isInside() == false) {
					requestExit = true;
					break;
				}
				continue;
			}
		}

		if (_kbhit()) {
			int key = _getch();
			if (key == 'z') {
				break; // exit from main loop
			}
			if (key == 'i') {
				int nEnemies = 0;
				int nPlayers = 0;
				int nBullets = 0;
				int nActiveBullets = 0;
				for (int i = 0; i < capacity; ++i)
				{
					if (gos[i] == nullptr) continue;
					if (dynamic_cast<Enemy*>(gos[i])) ++nEnemies;
					if (dynamic_cast<Player*>(gos[i])) ++nPlayers;
					if (dynamic_cast<Bullet*>(gos[i])) {
						++nBullets;
						if (static_cast<Bullet*>(gos[i])->checkFire() == true) {
							++nActiveBullets;
						}
					}
				}
				printf("total = %3d, enemies = %3d, players = %3d, bullets = %3d (%3d)\r", capacity, nEnemies, nPlayers, nBullets, nActiveBullets);
				Sleep(3000);
				continue;
			}

			for (int i = 0; i < capacity; i++)
			{
				if (gos[i] == nullptr) continue;
				gos[i]->process_input(key);
			}
		}
		gos = GameObject::getGameObjects();
		capacity = GameObject::getMaxGameObjects();
		for (int i = 0; i < capacity; i++)
		{
			if (gos[i] == nullptr) continue;
			gos[i]->update();
		}


		for (int i = 0; i < capacity; i++)
		{
			if (gos[i] == nullptr) continue;
			gos[i]->draw();
		}
		
		
		// display canvas to a monitor
		screen.render();
		Sleep(100);
	}
	printf("\n���������� ����Ǿ����ϴ�.\n");
	return 0;
}
