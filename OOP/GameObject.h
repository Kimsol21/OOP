#pragma once

class GameObjectManager;
class Screen;

class GameObject
{
	int		pos;
	char	shape[100]; // 0 ... 99
	static  GameObjectManager gameObjectManager;
	Screen& screen;

public:
	GameObject(Screen& screen, int pos, const char* shape);
	virtual ~GameObject() {}

	//getter ����
	int getPos() const { return pos; }
	const char* getShape() const { return shape; }
	Screen& getScreen() const { return screen; }

	static GameObject** getGameObjects();
	static int getMaxGameObjects();

	//setter ����
	void setPos(int pos) { this->pos = pos; }
	void setShape(const char* shape);

	bool isInside() const;
	void moveRight() { ++pos; }
	void moveLeft() { --pos; }

	virtual void process_input(int key) {}
	virtual void update() {}
	virtual void draw();
};

