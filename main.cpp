#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <raylib.h>
#include <string>
#include <vector>
#include <math.h>

typedef struct Timer 
{
    double startTime;   // Start time (seconds)
    double lifeTime;    // Lifetime (seconds)
} Timer;

void StartTimer(Timer* timer, double lifetime)
{
    timer->startTime = GetTime();
    timer->lifeTime = lifetime;
}

bool TimerDone(Timer timer)
{
    return GetTime() - timer.startTime >= timer.lifeTime;
}

double GetElapsed(Timer timer)
{
    return GetTime() - timer.startTime;
}

struct Bullet
{
    Texture2D texture;
    Vector2 velocity;

    Bullet(Texture2D& texture) { texture = LoadTexture("./art/bean.png"); };
};

int main()
{
    std::string title = "Coup d'état";

    const int screenWidth = 720;
    const int screenHeight = 480;
    InitWindow(screenWidth, screenHeight, "Coup d'état");
    SetTargetFPS(60);

    Vector2 center = { screenWidth * 0.5f, screenHeight * 0.5f };

    Texture2D mainTexture = LoadTexture("./art/menu.png");
    Texture2D backgroundTexture = LoadTexture("./art/map.png");
    Texture2D coopTexture = LoadTexture("./art/coop.png");
    Texture2D playerTexture = LoadTexture("./art/beanie.png");
    Texture2D beanTexture = LoadTexture("./art/bean.png");
    Texture2D enemyTexture = LoadTexture("./art/chicken.png");

    int numEnemies = 10;
    float radius = 400.0f;
    std::vector<Texture2D> enemies(numEnemies, enemyTexture);
    for (int i = 0; i < numEnemies; ++i)
    {
        float angle = 2 * PI * i / numEnemies;
        Vector2 position = { (float)(cos(angle) * radius + center.x), (float)(sin(angle) * radius + center.y) };
        enemies[i].setPosition(position);
    }
    float speed = 0.5f;

    std::vector<int> bulletsToRemove;
    std::vector<int> enemiesToRemove;

    Timer shootClock{};
    double shootCooldown = 250;

    Timer autoShootClock{};
    double autoShootCooldown = 1000;

    Timer chickenSoundClock{};
    double chickSoundCooldown = 1000;

    unsigned int score = 0;
    unsigned int highScore = 0;
    int playerLives = 3;
    int currentWave = 1;
    int bossHitCount = 0;
    int bossHealth = 5;
    int money = 0;

    bool paused = false;

    Font font = LoadFont("./fonts/adam.otf");

    DrawTextEx(font, "Coup d'état", Vector2{ center.x + 80.0f, center.y }, 48, 1, Color(RED))

    sf::Text menuText;
    menuText.setFont(font);
    menuText.setCharacterSize(48);
    menuText.setFillColor(sf::Color::Red);
    menuText.setPosition(center.x + 80.0f, center.y);
    menuText.setOrigin(menuText.getGlobalBounds().left * 0.5f, menuText.getGlobalBounds().top * 0.5f);
    bool onMenu = true;

    sf::Text spaceText;
    spaceText.setFont(font);
    spaceText.setCharacterSize(18);
    spaceText.setFillColor(sf::Color::Red);
    spaceText.setPosition(center.x + 140.0f, center.y + 160.0f);
    spaceText.setOrigin(spaceText.getGlobalBounds().left * 0.5f, spaceText.getGlobalBounds().top * 0.5f);
    int spaceAlpha = 255;
    bool isIncreasing = false;

    sf::Text moneyText;
    moneyText.setFont(font);
    moneyText.setCharacterSize(24);
    moneyText.setFillColor(sf::Color::White);
    moneyText.setPosition(0.0f, 90.0f);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(0.0f, 40.0f);

    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(0.0f, 30.0f);

    sf::Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(sf::Color::White);
    highScoreText.setPosition(0.0f, 10.0f);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::White);
    bool gameOver = false;

    sf::Text rText;
    rText.setFont(font);
    rText.setCharacterSize(18);
    rText.setFillColor(sf::Color::Red);
    int rAlpha = 255;

    sf::Text pausedText;
    pausedText.setFont(font);
    pausedText.setCharacterSize(48);
    pausedText.setFillColor(sf::Color::Red);

    sf::Text upgradeOneText;
    upgradeOneText.setFont(font);
    upgradeOneText.setCharacterSize(18);
    upgradeOneText.setFillColor(sf::Color::Red);
    int shootSpeedLevel = 0;

    sf::Text upgradeTwoText;
    upgradeTwoText.setFont(font);
    upgradeTwoText.setCharacterSize(18);
    upgradeTwoText.setFillColor(sf::Color::Red);
    int autoShootLevel = 0;
    int shotCount = 0;
    sf::Time shootSpeed = sf::milliseconds(1000);

    sf::Text upgradeThreeText;
    upgradeThreeText.setFont(font);
    upgradeThreeText.setCharacterSize(18);
    upgradeThreeText.setFillColor(sf::Color::Red);
    int shootDamageLevel = 1;

    sf::Text costTextOne;
    costTextOne.setFont(font);
    costTextOne.setCharacterSize(18);
    costTextOne.setFillColor(sf::Color::Red);

    sf::Text costTextTwo;
    costTextTwo.setFont(font);
    costTextTwo.setCharacterSize(18);
    costTextTwo.setFillColor(sf::Color::Red);

    sf::Text costTextThree;
    costTextThree.setFont(font);
    costTextThree.setCharacterSize(18);
    costTextThree.setFillColor(sf::Color::Red);

    sf::Text buyTextOne;
    buyTextOne.setFont(font);
    buyTextOne.setCharacterSize(18);
    buyTextOne.setFillColor(sf::Color::Red);

    sf::Text buyTextTwo;
    buyTextTwo.setFont(font);
    buyTextTwo.setCharacterSize(18);
    buyTextTwo.setFillColor(sf::Color::Red);

    sf::Text buyTextThree;
    buyTextThree.setFont(font);
    buyTextThree.setCharacterSize(18);
    buyTextThree.setFillColor(sf::Color::Red);

    //sf::RectangleShape buttonOne(Vector2(36.0f, 30.0f));
    //sf::RectangleShape buttonTwo(Vector2(36.0f, 30.0f));
    //sf::RectangleShape buttonThree(Vector2(36.0f, 30.0f));

    Rectangle buttonOne = { 0.0f, 0.0f, 36.0f, 30.0f };
    Rectangle buttonTwo = { 0.0f, 0.0f, 36.0f, 30.0f };
    Rectangle buttonThree = { 0.0f, 0.0f, 36.0f, 30.0f };

    sf::SoundBuffer bufferMenu;
    bufferMenu.loadFromFile("./sfx/menu.mp3");
    sf::Sound menuSound;
    menuSound.setBuffer(bufferMenu);

    menuSound.play();
    menuSound.setLoop(true);

    sf::SoundBuffer bufferBackground;
    bufferBackground.loadFromFile("./sfx/background_music.mp3");
    sf::Sound chickenBackground;
    chickenBackground.setBuffer(bufferBackground);
    bool gameMusicStarted = false;

    sf::SoundBuffer bufferKill;
    bufferKill.loadFromFile("./sfx/killchicken.mp3");
    sf::Sound chickenKill;
    chickenKill.setBuffer(bufferKill);

    sf::SoundBuffer bufferTender;
    bufferTender.loadFromFile("./sfx/tenders.mp3");
    sf::Sound chickenTender;
    chickenTender.setBuffer(bufferTender);

    sf::SoundBuffer bufferShoot;
    bufferShoot.loadFromFile("./sfx/shoot.wav");
    sf::Sound shootSound;
    shootSound.setBuffer(bufferShoot);

    while (window.isOpen())
    {
        if (gameMusicStarted)
        {
            chickenBackground.play();
            chickenBackground.setLoop(true);
            gameMusicStarted = false; // Just a hacky check
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (!onMenu && !gameOver && event.type == event.KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
            {
                if (!paused)
                {
                    paused = true;
                }

                else if (paused)
                {
                    paused = false;
                }
            }

            if (paused)
            {
                if (event.type == event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    Vector2 mouseCoords = window.mapPixelToCoords(mousePosition);

                    sf::CircleShape pos(1.0f);
                    pos.setPosition(mouseCoords);

                    if (buttonOne.getGlobalBounds().intersects(pos.getGlobalBounds()))
                    {
                        if (money >= (int)(((shootSpeedLevel + 1) * 5) * 0.75f))
                        {
                            money = money - (int)(((shootSpeedLevel + 1) * 5) * 0.75f);

                            shootSpeedLevel++;

                            shootCooldown = sf::milliseconds((int)(0.2f * shootSpeedLevel));
                        }
                        pos.setPosition(0.0f, 0.0f);
                    }

                    if (buttonTwo.getGlobalBounds().intersects(pos.getGlobalBounds()))
                    {
                        if (money >= (int)(((autoShootLevel + 1) * 5) * 0.75f))
                        {
                            money = money - (int)(((autoShootLevel + 1) * 5) * 0.75f);

                            autoShootLevel++;

                            if (autoShootLevel % 5 == 0)
                            {
                                shootSpeed -= sf::milliseconds(50);
                            }
                            else
                            {
                                shotCount++;
                            }
                        }

                        pos.setPosition(0.0f, 0.0f);
                    }

                    if (buttonThree.getGlobalBounds().intersects(pos.getGlobalBounds()))
                    {
                        if (money >= (int)(((shootDamageLevel + 1) * 5) * 0.75f))
                        {
                            money = money - (int)(((shootDamageLevel + 1) * 5) * 0.75f);

                            shootDamageLevel++;
                        }

                        pos.setPosition(0.0f, 0.0f);
                    }
                }
            }

        }

        if (onMenu)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                menuSound.stop();
                menuSound.setLoop(false);
                onMenu = false;
                gameMusicStarted = true;
            }
        }

        if (gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            chickenTender.stop();
            chickenBackground.play();
            chickenBackground.setLoop(true);
            numEnemies = 10;
            speed = 0.5f;
            score = 0;
            playerLives = 3;
            currentWave = 1;
            money = 0;
            bossHealth = 5;
            shootDamageLevel = 1;
            autoShootLevel = 0;
            shootSpeedLevel = 0;
            shootSpeed = sf::milliseconds(1000);
            shootCooldown = sf::milliseconds(1000);
            shotCount = 0;
            playerSprite.setPosition(center.x, center.y);
            playerSprite.setRotation(0.0f);
            gameOver = false;
        }

        if (!gameOver && !onMenu && !paused)
        {
            Vector2 movement(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                movement.y -= moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                movement.y += moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                movement.x -= moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                movement.x += moveSpeed;
            }
            playerSprite.move(movement);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootClock.getElapsedTime() >= shootCooldown)
            {
                shootSound.play();

                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                Vector2 mouseCoords = window.mapPixelToCoords(mousePosition);

                Vector2 playerPosition = playerSprite.getPosition();
                float dx = mouseCoords.x - playerPosition.x;
                float dy = mouseCoords.y - playerPosition.y;
                float angle = std::atan2(dy, dx) * 180 / PI;

                Bullet bullet(beanTexture);
                bullet.sprite.setPosition(playerPosition);

                float bulletSpeed = 5.0f;
                bullet.velocity.x = std::cos(angle * PI / 180.0f) * bulletSpeed;
                bullet.velocity.y = std::sin(angle * PI / 180.0f) * bulletSpeed;

                bullets.push_back(bullet);
                shootClock.restart();
            }
        }

        if (!onMenu && !paused)
        {
            Vector2 playerPos = playerSprite.getPosition();
            if (playerPos.x < 0.0f)
            {
                playerSprite.setPosition(0.0f, playerPos.y);
            }
            if (playerPos.x > center.x * 2.0f)
            {
                playerSprite.setPosition(center.x * 2.0f, playerPos.y);
            }
            if (playerPos.y < 0.0f)
            {
                playerSprite.setPosition(playerPos.x, 0.0f);
            }
            if (playerPos.y > center.y * 2.0f)
            {
                playerSprite.setPosition(playerPos.x, center.y * 2.0f);
            }

            Vector2 playerPosition = playerSprite.getPosition();
            for (auto& enemy : enemies)
            {

                Vector2 direction = playerPosition - enemy.getPosition();
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length != 0)
                {
                    direction /= length;

                    enemy.move(direction * speed);

                    if (direction.x > 0.0f)
                    {
                        enemy.setScale(-1.0f, 1.0f);
                    }

                    if (direction.x < 0.0f)
                    {
                        enemy.setScale(1.0f, 1.0f);
                    }
                }
            }

            if (autoShootLevel > 0 && autoShootClock.getElapsedTime() >= shootSpeed)
            {
                shootSound.play();

                for (int i = 0; i < shotCount; i++)
                {
                    Vector2 playerPosition = playerSprite.getPosition();
                    float angle = (i % 16) * 22.5f;

                    Bullet bullet(beanTexture);
                    bullet.sprite.setPosition(playerPosition);

                    float bulletSpeed = 5.0f;
                    bullet.velocity.x = std::cos(angle * PI / 180.0f) * bulletSpeed;
                    bullet.velocity.y = std::sin(angle * PI / 180.0f) * bulletSpeed;

                    bullets.push_back(bullet);
                }

                autoShootClock.restart();
            }

            for (auto& bullet : bullets)
            {
                bullet.sprite.move(bullet.velocity);
            }

            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](const Bullet& bullet)
                {
                    return bullet.sprite.getPosition().x < 0 ||
                        bullet.sprite.getPosition().y < 0 ||
                        bullet.sprite.getPosition().x > window.getSize().x ||
                        bullet.sprite.getPosition().y > window.getSize().y;
                }), bullets.end());

            for (size_t i = 0; i < bullets.size(); ++i)
            {
                for (size_t j = 0; j < enemies.size(); ++j)
                {
                    if (bullets[i].sprite.getGlobalBounds().intersects(enemies[j].getGlobalBounds()))
                    {
                        bulletsToRemove.push_back((int)i);

                        if (currentWave % 5 == 0)
                        {
                            bossHitCount += shootDamageLevel;

                            if (bossHitCount >= bossHealth)
                            {
                                enemiesToRemove.push_back((int)j);
                                bossHitCount = 0;

                                int roundMultiplier = (int)std::round(currentWave / 5.0f);
                                bossHealth = (int)((5 * ((roundMultiplier > 1) ? roundMultiplier : 1)) * 0.75f);
                            }
                            else
                            {
                                break;
                            }
                        }
                        enemiesToRemove.push_back((int)j);
                    }
                }
            }

            for (size_t i = 0; i < enemies.size(); ++i)
            {
                if (playerSprite.getGlobalBounds().intersects(enemies[i].getGlobalBounds()))
                {
                    playerLives -= 1;
                    score -= 1;
                    enemiesToRemove.push_back((int)i);
                }
            }

            std::sort(bulletsToRemove.rbegin(), bulletsToRemove.rend());
            bulletsToRemove.erase(std::unique(bulletsToRemove.begin(), bulletsToRemove.end()), bulletsToRemove.end());

            std::sort(enemiesToRemove.rbegin(), enemiesToRemove.rend());
            enemiesToRemove.erase(std::unique(enemiesToRemove.begin(), enemiesToRemove.end()), enemiesToRemove.end());

            for (int index : bulletsToRemove)
            {
                bullets.erase(bullets.begin() + index);
            }
            bulletsToRemove.clear();

            for (int index : enemiesToRemove)
            {
                if (currentWave % 5 == 0)
                {
                    score += 5;
                    money += (int)(5 + (currentWave * 0.1f));
                }
                else
                {
                    score += 1;
                    money += (int)(1 + (currentWave * 0.1f));
                }

                enemies.erase(enemies.begin() + index);
                chickenKill.play();
            }
            enemiesToRemove.clear();

            if (enemies.empty() && !gameOver)
            {
                numEnemies += 1;
                currentWave += 1;
                speed *= 1.05f;

                if (currentWave % 5 == 0)
                {
                    float angle = 2 * PI * 0 / 1;
                    Vector2 position = center + Vector2(std::cos(angle), std::sin(angle)) * radius;
                    enemySprite.setPosition(position);
                    enemies.push_back(enemySprite);
                }
                else
                {
                    for (int i = 0; i < numEnemies; ++i)
                    {
                        float angle = 2 * PI * i / numEnemies;
                        Vector2 position = center + Vector2(std::cos(angle), std::sin(angle)) * radius;
                        enemySprite.setPosition(position);
                        enemies.push_back(enemySprite);
                    }
                }
            }
        }

        if (score > highScore)
        {
            highScore = score;
        }

        menuText.setString("Coup d'état");
        spaceText.setString("Press 'Spacebar' to start");
        moneyText.setString("Eggs: " + std::to_string(money));
        scoreText.setString("Score: " + std::to_string(score));
        livesText.setString("Lives: " + std::to_string(playerLives));
        highScoreText.setString("Highscore: " + std::to_string(highScore));
        gameOverText.setString("Game Over");
        rText.setString("Press 'R' to restart");
        upgradeOneText.setString("Shoot Speed LvL: " + std::to_string(shootSpeedLevel));
        upgradeTwoText.setString("Auto Shoot LvL: " + std::to_string(autoShootLevel));
        upgradeThreeText.setString("Bullet Damage LvL: " + std::to_string(shootDamageLevel));
        buyTextOne.setString("Buy");
        buyTextTwo.setString("Buy");
        buyTextThree.setString("Buy");

        if (playerLives <= 0 && !gameOver)
        {
            chickenBackground.stop();
            chickenBackground.setLoop(false);
            enemies.clear();
            bullets.clear();
            enemiesToRemove.clear();
            bulletsToRemove.clear();
            gameOver = true;
            chickenTender.play();
        }

        window.clear();

        if (onMenu)
        {
            window.draw(mainSprite);

            window.draw(menuText);

            if (spaceAlpha >= 255)
            {
                isIncreasing = false;
            }
            if (spaceAlpha <= 0)
            {
                isIncreasing = true;
            }
            if (isIncreasing)
            {
                spaceAlpha += 3;
            }
            if (!isIncreasing)
            {
                spaceAlpha -= 3;
            }

            spaceText.setFillColor(sf::Color(255, 0, 0, spaceAlpha));
            window.draw(spaceText);
        }

        if (!gameOver && !onMenu && !paused)
        {
            window.draw(backgroundSprite);

            window.draw(coopSprite);

            window.draw(playerSprite);

            for (const auto& bullet : bullets)
            {
                window.draw(bullet.sprite);
            }

            for (const auto& enemy : enemies)
            {

                if (currentWave % 5 == 0)
                {
                    enemies[0].setScale(2.5f, 2.5f);
                }
                window.draw(enemy);
            }

            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(0.0f, 60.0f);
            scoreText.setOrigin(0.0f, 0.0f);
            window.draw(scoreText);

            window.draw(livesText);

            highScoreText.setFillColor(sf::Color::White);
            highScoreText.setPosition(0.0f, 0.0f);
            highScoreText.setOrigin(0.0f, 0.0f);
            window.draw(highScoreText);

            moneyText.setFillColor(sf::Color::White);
            moneyText.setPosition(0.0f, 90.0f);
            moneyText.setOrigin(0.0f, 0.0f);
            window.draw(moneyText);
        }

        if (gameOver && !onMenu && !paused)
        {
            window.draw(mainSprite);

            gameOverText.setPosition(center.x, center.y * 0.75f);
            gameOverText.setOrigin(gameOverText.getGlobalBounds().width * 0.5f, gameOverText.getGlobalBounds().top * 0.5f);
            gameOverText.setFillColor(sf::Color::Red);
            window.draw(gameOverText);

            scoreText.setFillColor(sf::Color::Red);
            scoreText.setPosition(center.x, center.y * 1.50f);
            scoreText.setOrigin(scoreText.getGlobalBounds().width * 0.5f, scoreText.getGlobalBounds().top * 0.5f);
            window.draw(scoreText);

            highScoreText.setFillColor(sf::Color::Red);
            highScoreText.setPosition(center.x, center.y * 1.35f);
            highScoreText.setOrigin(highScoreText.getGlobalBounds().width * 0.5f, highScoreText.getGlobalBounds().top * 0.5f);
            window.draw(highScoreText);

            if (rAlpha >= 255)
            {
                isIncreasing = false;
            }
            if (rAlpha <= 0)
            {
                isIncreasing = true;
            }
            if (isIncreasing)
            {
                rAlpha += 3;
            }
            if (!isIncreasing)
            {
                rAlpha -= 3;
            }

            rText.setFillColor(sf::Color(255, 0, 0, rAlpha));
            rText.setPosition(center.x + 50.0f, center.y * 1.8f);
            rText.setOrigin(rText.getGlobalBounds().left * 0.5f, rText.getGlobalBounds().top * 0.5f);

            window.draw(rText);
        }

        if (paused)
        {
            moneyText.setFillColor(sf::Color::Red);
            moneyText.setPosition(center.x + 65.0f, center.y * 0.25f);
            moneyText.setOrigin(moneyText.getGlobalBounds().left * 0.5f, moneyText.getGlobalBounds().top * 0.5f);
            window.draw(moneyText);

            upgradeOneText.setPosition(center.x + 65.0f, center.y * 0.5f);
            upgradeOneText.setOrigin(upgradeOneText.getGlobalBounds().left * 0.5f, upgradeOneText.getGlobalBounds().top * 0.5f);
            window.draw(upgradeOneText);

            costTextOne.setPosition(center.x + 65.0f, center.y * 0.65f);
            costTextOne.setOrigin(costTextOne.getGlobalBounds().left * 0.5f, costTextOne.getGlobalBounds().top * 0.5f);
            costTextOne.setString("Cost: " + std::to_string(static_cast<int>(((shootSpeedLevel + 1) * 5) * 0.75f)) + " Eggs");
            window.draw(costTextOne);

            buyTextOne.setPosition(center.x - 65.0f, center.y * 0.57f);
            buyTextOne.setOrigin(buyTextOne.getGlobalBounds().left * 0.5f, buyTextOne.getGlobalBounds().top * 0.5f);
            window.draw(buyTextOne);

            buttonOne.setFillColor(sf::Color::Transparent);
            buttonOne.setOutlineColor(sf::Color::Red);
            buttonOne.setOutlineThickness(2.0f);
            buttonOne.setPosition(center.x - 165.0f, center.y * 0.36f);
            window.draw(buttonOne);

            upgradeTwoText.setPosition(center.x + 65.0f, center.y);
            upgradeTwoText.setOrigin(upgradeTwoText.getGlobalBounds().left * 0.5f, upgradeTwoText.getGlobalBounds().top * 0.5f);
            window.draw(upgradeTwoText);

            costTextTwo.setPosition(center.x + 65.0f, center.y * 1.15f);
            costTextTwo.setOrigin(costTextTwo.getGlobalBounds().left * 0.5f, costTextTwo.getGlobalBounds().top * 0.5f);
            costTextTwo.setString("Cost: " + std::to_string(static_cast<int>(((autoShootLevel + 1) * 5) * 0.75f)) + " Eggs");
            window.draw(costTextTwo);

            buyTextTwo.setPosition(center.x - 65.0f, center.y * 1.07f);
            buyTextTwo.setOrigin(buyTextTwo.getGlobalBounds().left * 0.5f, buyTextTwo.getGlobalBounds().top * 0.5f);
            window.draw(buyTextTwo);

            buttonTwo.setFillColor(sf::Color::Transparent);
            buttonTwo.setOutlineColor(sf::Color::Red);
            buttonTwo.setOutlineThickness(2.0f);
            buttonTwo.setPosition(center.x - 165.0f, center.y * 0.69f);
            window.draw(buttonTwo);

            upgradeThreeText.setPosition(center.x + 65.0f, center.y * 1.5f);
            upgradeThreeText.setOrigin(upgradeThreeText.getGlobalBounds().left * 0.5f, upgradeThreeText.getGlobalBounds().top * 0.5f);
            window.draw(upgradeThreeText);

            costTextThree.setPosition(center.x + 65.0f, center.y * 1.65f);
            costTextThree.setOrigin(costTextThree.getGlobalBounds().left * 0.5f, costTextThree.getGlobalBounds().top * 0.5f);
            costTextThree.setString("Cost: " + std::to_string(static_cast<int>(((shootDamageLevel + 1) * 5) * 0.75f)) + " Eggs");
            window.draw(costTextThree);

            buyTextThree.setPosition(center.x - 65.0f, center.y * 1.57f);
            buyTextThree.setOrigin(buyTextThree.getGlobalBounds().left * 0.5f, buyTextThree.getGlobalBounds().top * 0.5f);
            window.draw(buyTextThree);

            buttonThree.setFillColor(sf::Color::Transparent);
            buttonThree.setOutlineColor(sf::Color::Red);
            buttonThree.setOutlineThickness(2.0f);
            buttonThree.setPosition(center.x - 165.0f, center.y * 1.03f);
            window.draw(buttonThree);
        }

        window.display();
    }

    return 0;
}
