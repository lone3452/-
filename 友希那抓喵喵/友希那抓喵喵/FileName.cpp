#include <graphics.h>
#include <conio.h>
#include <ctime>
#include <list>
#include <string>
#include <vector>

#pragma comment(lib, "MSIMG32.LIB") // 支持透明图层

// 游戏窗口和蛇的常量
const int WIDTH = 1000;   // 游戏窗口宽度
const int HEIGHT = 800;   // 游戏窗口高度
const int BLOCK_SIZE = 80;  // 每个块的大小
const int GRID_WIDTH = WIDTH / BLOCK_SIZE;
const int GRID_HEIGHT = HEIGHT / BLOCK_SIZE;

enum Direction { UP, DOWN, LEFT, RIGHT };

// 蛇和水果结构体
struct Block {
    int x, y;
};

std::list<Block> snake;
Block fruit, superFood;
Direction dir = RIGHT;
int score = 0;
bool running = true;
bool truncationMode = false;  // 是否启用截断模式
bool superFoodGenerated = false;  // 标记超级食物是否已生成
std::vector<Block> obstacles;  // 障碍物列表

// 加载图像资源
IMAGE imgSnakeHead, imgSnakeBody, imgFruit, imgSuperFood, imgMenuBg, imgGameOver, imgBackground, imgObstacle;

// 初始化图像资源
void loadImages() {
    loadimage(&imgSnakeHead, _T("head.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgSnakeBody, _T("body.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgFruit, _T("food.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgSuperFood, _T("superfood.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgObstacle, _T("barrier.png"), BLOCK_SIZE, BLOCK_SIZE);
    loadimage(&imgMenuBg, _T("menu_bg.png"), WIDTH, HEIGHT);
    loadimage(&imgGameOver, _T("gameover.png"), WIDTH, HEIGHT);
    loadimage(&imgBackground, _T("background.png"), WIDTH, HEIGHT); // 加载背景图
}

// 生成随机水果
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

// 生成超级食物
void generateSuperFood() {
    while (true) {
        superFood.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        superFood.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
        bool conflict = false;

        // 确保超级食物不会与蛇身、普通食物或障碍物冲突
        for (auto& s : snake) {
            if (s.x == superFood.x && s.y == superFood.y) {
                conflict = true;
                break;
            }
        }

        // 确保超级食物不与普通食物冲突
        if (superFood.x == fruit.x && superFood.y == fruit.y) {
            conflict = true;
        }

        // 确保超级食物不与障碍物冲突
        for (auto& obs : obstacles) {
            if (obs.x == superFood.x && obs.y == superFood.y) {
                conflict = true;
                break;
            }
        }

        // 没有冲突就退出
        if (!conflict) break;
    }
    superFoodGenerated = true; // 设置为已生成
}

// 生成障碍物
void generateObstacle() {
    Block newObstacle;
    while (true) {
        newObstacle.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        newObstacle.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
        bool conflict = false;
        // 检查障碍物是否与蛇或已有障碍物冲突
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

// 初始化游戏
void initGame() {
    snake.clear();
    snake.push_back({ 5 * BLOCK_SIZE, 5 * BLOCK_SIZE });
    dir = RIGHT;
    score = 0;
    generateFruit();
    superFoodGenerated = false;  // 超级食物没有生成
    obstacles.clear(); // 清空障碍物
}

// 检查碰撞（蛇是否碰到边界、蛇身或障碍物）
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

// 更新游戏状态
void update() {
    Block head = snake.front();
    switch (dir) {
    case UP:    head.y -= BLOCK_SIZE; break;
    case DOWN:  head.y += BLOCK_SIZE; break;
    case LEFT:  head.x -= BLOCK_SIZE; break;
    case RIGHT: head.x += BLOCK_SIZE; break;
    }

    // 处理碰撞，如果是截断模式则不死
    if (checkCollision(head.x, head.y)) {
        if (truncationMode) {
            // 截断模式：删除与蛇头相碰的部分（从蛇尾开始删除）
            while (snake.back().x != head.x || snake.back().y != head.y) {
                snake.pop_back();
            }
            snake.pop_back(); // 删除最后一个与蛇头相碰的部分
        }
        else {
            running = false; // 否则直接结束游戏
        }
        return;
    }

    snake.push_front(head);
    if (head.x == fruit.x && head.y == fruit.y) {
        score++;
        generateFruit();
    }
    else if (head.x == superFood.x && head.y == superFood.y) {
        score += 3;  // 吃到超级食物增加3分
        superFoodGenerated = false;  // 超级食物已被吃掉

        // 增加蛇的长度 3 格
        for (int i = 0; i < 3; i++) {
            snake.push_back(snake.back());  // 把蛇尾复制到蛇身，模拟增加长度
        }
    }
    else {
        snake.pop_back();
    }
}

// 绘制游戏界面
void draw() {
    setbkcolor(WHITE);
    cleardevice();

    // 画背景
    putimage(0, 0, &imgBackground);

    // 画果子
    putimage(fruit.x, fruit.y, &imgFruit);

    // 如果超级食物已生成，画超级食物
    if (superFoodGenerated) {
        putimage(superFood.x, superFood.y, &imgSuperFood);
    }

    // 画蛇
    bool isHead = true;
    for (auto& s : snake) {
        if (isHead) {
            putimage(s.x, s.y, &imgSnakeHead);  // 蛇头
            isHead = false;
        }
        else {
            putimage(s.x, s.y, &imgSnakeBody);  // 蛇身
        }
    }

    // 画障碍物
    for (auto& obs : obstacles) {
        putimage(obs.x, obs.y, &imgObstacle);  // 障碍物
    }

    // 显示分数
    settextcolor(BLACK);
    settextstyle(25, 0, _T("微软雅黑"));
    TCHAR str[50];
    _stprintf_s(str, _T("分数：%d"), score);
    outtextxy(10, 10, str);

    // 显示操作说明
    int offsetX = GRID_WIDTH * BLOCK_SIZE + 20; // UI区域左侧距离
    int offsetY = 80; // 起始Y坐标

    // 检查右边UI区域是否超出屏幕
    if (offsetX + 300 > WIDTH) {
        offsetX = WIDTH - 300;  // 防止UI超出右边界
    }

    outtextxy(offsetX, offsetY, L"操作说明:");
    offsetY += 40; // 增加间距
    outtextxy(offsetX, offsetY, L"WASD - 控制方向");
    offsetY += 40;
    outtextxy(offsetX, offsetY, L"B - 添加障碍物");
    offsetY += 40;
    outtextxy(offsetX, offsetY, L"V - 添加超级食物");
    offsetY += 40;
    outtextxy(offsetX, offsetY, L"ESC - 返回菜单");
}

// 处理键盘输入
void handleInput() {
    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
        case 'w': if (dir != DOWN) dir = UP; break;
        case 's': if (dir != UP) dir = DOWN; break;
        case 'a': if (dir != RIGHT) dir = LEFT; break;
        case 'd': if (dir != LEFT) dir = RIGHT; break;
        case 'b': generateObstacle(); break;  // 按B添加障碍物
        case 'v': if (!superFoodGenerated) generateSuperFood(); break; // 按V增加超级食物
        case 27: running = false; break;      // 按Esc返回菜单
        }
    }
}

// 绘制菜单界面
void drawMenu() {
    putimage(0, 0, &imgMenuBg);
    settextcolor(WHITE);
    settextstyle(36, 0, _T("微软雅黑"));
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2 - 60, L"欢迎来到友希那抓喵喵!");
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2, L"1. 开始游戏");
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2 + 40, L"2. 截断模式");
    outtextxy(WIDTH / 2 - 120, HEIGHT / 2 + 80, L"3. 退出游戏");
}

// 绘制游戏结束界面
// 绘制游戏结束界面
void drawGameOver() {

    putimage(0, 0, &imgGameOver);  // 显示游戏结束图像

    // 设置文本样式
    settextcolor(BLACK);  // 使用黑色文字
    settextstyle(36, 0, _T("微软雅黑"));

    // 显示分数
    wchar_t scoreText[50];
    swprintf(scoreText, 50, L"游戏结束！您的分数: %d", score);

    // 显示文本，确保显示在屏幕中间
    int textWidth = textwidth(scoreText);
    int textHeight = textheight(scoreText);
    outtextxy((WIDTH - textWidth) / 2, (HEIGHT - textHeight) / 2, scoreText);
}



// 主程序
int main() {
    initgraph(WIDTH, HEIGHT);
    srand((unsigned)time(0));

    loadImages();

    // 绘制菜单界面并等待用户输入
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
                return 0; // 退出游戏
            }
        }
        Sleep(10);  // 避免CPU占用过高
    }

    // 开始游戏
    while (running) {
        handleInput();
        update();
        draw();
        Sleep(100);  // 控制游戏速度
    }

    // 游戏结束，显示游戏结束界面
    drawGameOver();
    _getch();  // 等待用户按键

    closegraph();
    return 0;
}
