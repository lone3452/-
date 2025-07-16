#include <graphics.h>
#include <conio.h>
#include <ctime>
#include <list>
#include <string>
#include <vector>

#pragma comment(lib, "MSIMG32.LIB") // ֧��͸��ͼ��

// ��Ϸ���ں��ߵĳ���
const int WIDTH = 1000;   // ��Ϸ���ڿ��
const int HEIGHT = 800;   // ��Ϸ���ڸ߶�
const int BLOCK_SIZE = 80;  // ÿ����Ĵ�С
const int GRID_WIDTH = WIDTH / BLOCK_SIZE;
const int GRID_HEIGHT = HEIGHT / BLOCK_SIZE;

enum Direction { UP, DOWN, LEFT, RIGHT };

// �ߺ�ˮ���ṹ��
struct Block {
    int x, y;
};

std::list<Block> snake;
Block fruit, superFood;
Direction dir = RIGHT;
int score = 0;
bool running = true;
bool truncationMode = false;  // �Ƿ����ýض�ģʽ
bool superFoodGenerated = false;  // ��ǳ���ʳ���Ƿ�������
std::vector<Block> obstacles;  // �ϰ����б�

// ����ͼ����Դ
IMAGE imgSnakeHead, imgSnakeBody, imgFruit, imgSuperFood, imgMenuBg, imgGameOver, imgBackground, imgObstacle;

// ��ʼ��ͼ����Դ
void loadImages() {
    loadimage(&imgSnakeHead, _T("head.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgSnakeBody, _T("body.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgFruit, _T("food.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgSuperFood, _T("superfood.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgObstacle, _T("barrier.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgMenuBg, _T("menu_bg.png"), WIDTH, HEIGHT);
    loadimage(&imgGameOver, _T("gameover.png"), WIDTH, HEIGHT);
    loadimage(&imgBackground, _T("background.png"), WIDTH, HEIGHT); // ���ر���ͼ
}

// �������ˮ��
void generateFruit() {
    while (true) {
        fruit.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        fruit.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
        bool conflict = false;
        for (auto& s : snake) {
            if (s.x == fruit.x && s.y == fruit.y) {
                conflict = true;
                break;
            }
        }
        if (!conflict) break;
    }
}

// ���ɳ���ʳ��
void generateSuperFood() {
    while (true) {
        superFood.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        superFood.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
        bool conflict = false;

        // ȷ������ʳ�ﲻ����������ͨʳ����ϰ����ͻ
        for (auto& s : snake) {
            if (s.x == superFood.x && s.y == superFood.y) {
                conflict = true;
                break;
            }
        }

        // ȷ������ʳ�ﲻ����ͨʳ���ͻ
        if (superFood.x == fruit.x && superFood.y == fruit.y) {
            conflict = true;
        }

        // ȷ������ʳ�ﲻ���ϰ����ͻ
        for (auto& obs : obstacles) {
            if (obs.x == superFood.x && obs.y == superFood.y) {
                conflict = true;
                break;
            }
        }

        // û�г�ͻ���˳�
        if (!conflict) break;
    }
    superFoodGenerated = true; // ����Ϊ������
}

// �����ϰ���
void generateObstacle() {
    Block newObstacle;
    while (true) {
        newObstacle.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        newObstacle.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
        bool conflict = false;
        // ����ϰ����Ƿ����߻������ϰ����ͻ
        for (auto& s : snake) {
            if (s.x == newObstacle.x && s.y == newObstacle.y) {
                conflict = true;
                break;
            }
        }
        for (auto& obs : obstacles) {
            if (obs.x == newObstacle.x && obs.y == newObstacle.y) {
                conflict = true;
                break;
            }
        }
        if (!conflict) break;
    }
    obstacles.push_back(newObstacle);
}

// ��ʼ����Ϸ
void initGame() {
    snake.clear();
    snake.push_back({ 5 * BLOCK_SIZE, 5 * BLOCK_SIZE });
    dir = RIGHT;
    score = 0;
    generateFruit();
    superFoodGenerated = false;  // ����ʳ��û������
    obstacles.clear(); // ����ϰ���
}

// �����ײ�����Ƿ������߽硢������ϰ��
bool checkCollision(int x, int y) {
    if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return true;
    for (auto it = ++snake.begin(); it != snake.end(); ++it) {
        if (it->x == x && it->y == y) return true;
    }
    for (auto& obs : obstacles) {
        if (obs.x == x && obs.y == y) return true;
    }
    return false;
}

// ������Ϸ״̬
void update() {
    Block head = snake.front();
    switch (dir) {
    case UP:    head.y -= BLOCK_SIZE; break;
    case DOWN:  head.y += BLOCK_SIZE; break;
    case LEFT:  head.x -= BLOCK_SIZE; break;
    case RIGHT: head.x += BLOCK_SIZE; break;
    }

    // ������ײ������ǽض�ģʽ����
    if (checkCollision(head.x, head.y)) {
        if (truncationMode) {
            // �ض�ģʽ��ɾ������ͷ�����Ĳ��֣�����β��ʼɾ����
            while (snake.back().x != head.x || snake.back().y != head.y) {
                snake.pop_back();
            }
            snake.pop_back(); // ɾ�����һ������ͷ�����Ĳ���
        }
        else {
            running = false; // ����ֱ�ӽ�����Ϸ
        }
        return;
    }

    snake.push_front(head);
    if (head.x == fruit.x && head.y == fruit.y) {
        score++;
        generateFruit();
    }
    else if (head.x == superFood.x && head.y == superFood.y) {
        score += 3;  // �Ե�����ʳ������3��
        superFoodGenerated = false;  // ����ʳ���ѱ��Ե�

        // �����ߵĳ��� 3 ��
        for (int i = 0; i < 3; i++) {
            snake.push_back(snake.back());  // ����β���Ƶ�����ģ�����ӳ���
        }
    }
    else {
        snake.pop_back();
    }
}

// ������Ϸ����
void draw() {
    setbkcolor(WHITE);
    cleardevice();

    // ������
    putimage(0, 0, &imgBackground);

    // ������
    putimage(fruit.x, fruit.y, &imgFruit);

    // �������ʳ�������ɣ�������ʳ��
    if (superFoodGenerated) {
        putimage(superFood.x, superFood.y, &imgSuperFood);
    }

    // ����
    bool isHead = true;
    for (auto& s : snake) {
        if (isHead) {
            putimage(s.x, s.y, &imgSnakeHead);  // ��ͷ
            isHead = false;
        }
        else {
            putimage(s.x, s.y, &imgSnakeBody);  // ����
        }
    }

    // ���ϰ���
    for (auto& obs : obstacles) {
        putimage(obs.x, obs.y, &imgObstacle);  // �ϰ���
    }

    // ��ʾ����
    settextcolor(BLACK);
    settextstyle(25, 0, _T("΢���ź�"));
    TCHAR str[50];
    _stprintf_s(str, _T("������%d"), score);
    outtextxy(10, 10, str);

    // ��ʾ����˵��
    int offsetX = GRID_WIDTH * BLOCK_SIZE + 20; // UI����������
    int offsetY = 80; // ��ʼY����

    // ����ұ�UI�����Ƿ񳬳���Ļ
    if (offsetX + 300 > WIDTH) {
        offsetX = WIDTH - 300;  // ��ֹUI�����ұ߽�
    }

    outtextxy(offsetX, offsetY, L"����˵��:");
    offsetY += 40; // ���Ӽ��
    outtextxy(offsetX, offsetY, L"WASD - ���Ʒ���");
    offsetY += 40;
    outtextxy(offsetX, offsetY, L"B - ����ϰ���");
    offsetY += 40;
    outtextxy(offsetX, offsetY, L"V - ��ӳ���ʳ��");
    offsetY += 40;
    outtextxy(offsetX, offsetY, L"ESC - ���ز˵�");
}

// �����������
void handleInput() {
    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
        case 'w': if (dir != DOWN) dir = UP; break;
        case 's': if (dir != UP) dir = DOWN; break;
        case 'a': if (dir != RIGHT) dir = LEFT; break;
        case 'd': if (dir != LEFT) dir = RIGHT; break;
        case 'b': generateObstacle(); break;  // ��B����ϰ���
        case 'v': if (!superFoodGenerated) generateSuperFood(); break; // ��V���ӳ���ʳ��
        case 27: running = false; break;      // ��Esc���ز˵�
        }
    }
}

// ���Ʋ˵�����
void drawMenu() {
    putimage(0, 0, &imgMenuBg);
    settextcolor(WHITE);
    settextstyle(36, 0, _T("΢���ź�"));
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2 - 60, L"��ӭ������ϣ��ץ����!");
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2, L"1. ��ʼ��Ϸ");
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2 + 40, L"2. �ض�ģʽ");
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2 + 80, L"3. �˳���Ϸ");
}

// ������Ϸ��������
// ������Ϸ��������
void drawGameOver() {

    putimage(0, 0, &imgGameOver);  // ��ʾ��Ϸ����ͼ��

    // �����ı���ʽ
    settextcolor(BLACK);  // ʹ�ú�ɫ����
    settextstyle(36, 0, _T("΢���ź�"));

    // ��ʾ����
    wchar_t scoreText[50];
    swprintf(scoreText, 50, L"��Ϸ���������ķ���: %d", score);

    // ��ʾ�ı���ȷ����ʾ����Ļ�м�
    int textWidth = textwidth(scoreText);
    int textHeight = textheight(scoreText);
    outtextxy((WIDTH - textWidth) / 2, (HEIGHT - textHeight) / 2, scoreText);
}



// ������
int main() {
    initgraph(WIDTH, HEIGHT);
    srand((unsigned)time(0));

    loadImages();

    // ���Ʋ˵����沢�ȴ��û�����
    while (true) {
        drawMenu();
        if (_kbhit()) {
            char ch = _getch();
            if (ch == '1') {
                initGame();
                break;
            }
            else if (ch == '2') {
                truncationMode = true;
                initGame();
                break;
            }
            else if (ch == '3') {
                closegraph();
                return 0; // �˳���Ϸ
            }
        }
        Sleep(10);  // ����CPUռ�ù���
    }

    // ��ʼ��Ϸ
    while (running) {
        handleInput();
        update();
        draw();
        Sleep(100);  // ������Ϸ�ٶ�
    }

    // ��Ϸ��������ʾ��Ϸ��������
    drawGameOver();
    _getch();  // �ȴ��û�����

    closegraph();
    return 0;
}
