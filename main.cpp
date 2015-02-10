/*
    Aaron Browne
    COP1334 - 758327
    TR 9:00 - 10:40
    Through the Looking Glass Program
*/

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

struct pos
{
    int x;
    int y;
};

struct Bunny
{
    unsigned char icon;
    pos position;
    char direction;
    int biteCount;
    int pelletCount;
    bool haveGun;
    int health;
    int dmgTaken;
    int actionPts;
    int withoutPellet;

    void initialize()
    {
        icon = '@';
        direction = 'N';
        health = 10;
        dmgTaken = 0;
        position.x = 2;
        position.y = 2;
        actionPts = 3;
        biteCount = 5;
        pelletCount = 0;
        haveGun = false;
        withoutPellet = 0;

    }
};

struct Carrot
{
    unsigned char icon;
    pos position;
    char direction;
    int health;
    int dmgTaken;
    int actionPts;

    void initialize()
    {
        icon = 193;
        direction = 'N';
        health = 3;
        dmgTaken = 0;
        actionPts = 2;
    }
};

//Global Constants
const int DIM = 13;
const int BLANK = 250;

void fillGrid(unsigned char[][DIM]); //fills the grid with boarders
void printGrid(unsigned char[][DIM]); 
string directmsg(Bunny); //displays a message on player's direction
int menu(); //displays main menu
void moveFwd(unsigned char[][DIM], Bunny &); 
void turnLeft(Bunny &);
void turnRight(Bunny &);
void bunnyBite(Bunny &, Carrot &, Carrot &, unsigned char [][DIM]);
void biteCalc(Bunny &, Carrot &);
void carrotSpawn(Carrot &, unsigned char [][DIM]); //determines where carrots spawn
void carrotCalc(Bunny &, Carrot &, unsigned char [][DIM]); //Enemy AI
void itemSpawn(unsigned char [][DIM]); //determines where items spawn
void pickUpItem(unsigned char [][DIM], Bunny &);
void itemCalc(unsigned char [][DIM], Bunny &, char);
void shootCalc(unsigned char [][DIM], Bunny &, Carrot &, Carrot &); //determines shooting mechanism

int main()
{
    unsigned char playGrid[DIM][DIM];
    srand(time(0)); //seed random valiable
    int itemChance = rand() % 2 + 1; //50% chance of item spawn

    fillGrid(playGrid);

    Bunny player;
    player.initialize();
    playGrid[player.position.x][player.position.y] = player.icon;

    Carrot enemyA;
    enemyA.initialize();
    carrotSpawn(enemyA, playGrid);

    Carrot enemyB;
    enemyB.initialize();
    carrotSpawn(enemyB, playGrid);
    enemyB.actionPts = 3; //This is a faster carrot

    cout << endl << directmsg(player) << endl;

    while(player.health > 0 && player.withoutPellet < 3)
    {
        printGrid(playGrid);

        while (player.actionPts > 0)
        {
            int action = menu();

            switch(action)
            {
            case 1:
            {
                moveFwd(playGrid, player);
            }
            break;
            case 2:
            {
                turnLeft(player);
            }
            break;
            case 3:
            {
                turnRight(player);
            }
            break;
            case 4:
            {
                if (player.biteCount > 0)
                    bunnyBite(player, enemyA, enemyB, playGrid);
                else
                    cout << "You're out of bites!\n";
            }
            break;
            case 5:
            {
                pickUpItem(playGrid, player);
            }
            break;
            case 6:
            {
                if (player.haveGun == true)
                    shootCalc(playGrid, player, enemyA, enemyB);
                else
                    cout << "\nYou don't have a gun!" << endl;
            }
            break;
            case 7:
            {
                return 0;
                break;
            }
            break;
            }
            printGrid(playGrid);
            cout << directmsg(player);
        }
        player.actionPts += 3;

        while (enemyA.actionPts > 0 && enemyA.health > 0)
        {
            carrotCalc(player, enemyA, playGrid);
            system("pause");
            printGrid(playGrid);
        }
        enemyA.actionPts += 2;

        if (enemyA.health <= 0) //if enemyA dies, it is placed into the corner and turns into a corner
        {
            playGrid[enemyA.position.y][enemyA.position.x] = BLANK;
            enemyA.position.x = 0;
            enemyA.position.y = 0;
            enemyA.icon = 201;
            playGrid[enemyA.position.y][enemyA.position.x] = enemyA.icon;
        }


        while (enemyB.actionPts > 0 && enemyB.health > 0)
        {
            carrotCalc(player, enemyB, playGrid);
            system("pause");

            printGrid(playGrid);
        }
        enemyB.actionPts += 3;

        if (enemyB.health <= 0)
        {
            playGrid[enemyB.position.y][enemyB.position.x] = BLANK;
            enemyB.position.x = DIM;
            enemyB.position.y = DIM;
            enemyB.icon = 188;
            playGrid[enemyB.position.y][enemyB.position.x] = enemyB.icon;

        }

        itemChance = rand() % 2 + 1;

        if (itemChance == 2)
        {
            itemSpawn(playGrid);
            printGrid(playGrid);
        }

        if (player.biteCount <= 0)
        {
            player.withoutPellet += 1; // if without pellets for 3 turns, it's game over
        }
        system("clear");

    }

    cout << "Game Over!" << endl;

    if (player.withoutPellet >= 3)
        cout << "You've ran out of pellets for too long!";
    else 
        cout << "You have ran out of health!";

    return 0;
}

void fillGrid(unsigned char playGrid[][DIM])
{
    //blanks
    for (int ROW = 0; ROW < DIM; ROW++) 
    {
        for (int COL = 0; COL < DIM; COL++)
        {
            playGrid[ROW][COL] = 250;
        }
    }

    //boarders
    for (int topROW = 0; topROW < DIM; topROW++)
    {
        playGrid[0][topROW] = 205;
    }

    for (int botROW = 0; botROW < DIM; botROW++)
    {
        playGrid[12][botROW] = 205;
    }


    for (int lCOL = 0; lCOL < DIM; lCOL++)
    {
        playGrid[lCOL][0] = 186;
    }

    for (int rCOL = 0; rCOL < DIM; rCOL++)
    {
        playGrid[rCOL][12] = 186;
    }

    //corners
    playGrid[0][0] = 201;
    playGrid[12][0] = 200;
    playGrid[0][12] = 187;
    playGrid[12][12] = 188;
}

void printGrid(unsigned char playGrid[][DIM])
{

    for (int r = 0; r < DIM; r++)
    {
        for (int c = 0; c < DIM; c++)
        {
            cout << playGrid[r][c];
        }
        cout << endl;
    }

}

string directmsg(Bunny player)
{
    if (player.direction == 'N')
        return "\nYou are now facing North.\n";
    else if (player.direction == 'E')
        return "\nYou are now facing East.\n";
    else if (player.direction == 'W')
        return "\nYou are now facing West.\n";
    else
        return "\nYou are now facing South.\n";
}

int menu()
{
    int menuAnswer = 0;

    cout << "\nWhat would you like to do?" <<
         "\n1. Move forward"<<
         "\n2. Turn left"<<
         "\n3. Turn right" <<
         "\n4. Bite"<<
         "\n5. Pick up item"<<
         "\n6. Shoot gun"<<
         "\n7. Exit"<<
         "\nEnter your answer (1-7): ";
    cin >> menuAnswer;

    while (menuAnswer < 1 || menuAnswer > 7 || !cin) //clears buffer and fix cin if input is invalid
    {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Invalid answer. Please try again: ";
        cin >> menuAnswer;
    }

    return menuAnswer;
}

void moveFwd(unsigned char playGrid[][DIM], Bunny & player)
{

    switch (player.direction)
    {
    case 'N':
        if (playGrid[player.position.y - 1][player.position.x] == BLANK)
        {
            playGrid[player.position.y][player.position.x] = BLANK; //old position
            playGrid[player.position.y - 1][player.position.x] = player.icon; //new position
            player.position.y -= 1; //update within structure
            player.actionPts -= 1;
        }
        else
            cout << "\nDid not move north.\n";
        break;
    case 'E':
        if (playGrid[player.position.y][player.position.x + 1] == BLANK)
        {
            playGrid[player.position.y][player.position.x] = BLANK;
            playGrid[player.position.y][player.position.x + 1] = player.icon;
            player.position.x += 1;
            player.actionPts -= 1;
        }
        else
            cout << "\nDid not move East.\n";
        break;
    case 'W':
        if (playGrid[player.position.y][player.position.x - 1] == BLANK)
        {
            playGrid[player.position.y][player.position.x] = BLANK;
            playGrid[player.position.y][player.position.x - 1] = player.icon;
            player.position.x -= 1;
            player.actionPts -= 1;
        }
        else
            cout << "\nDid not move West.\n";
        break;
    case 'S':
        if (playGrid[player.position.y + 1][player.position.x] == BLANK)
        {
            playGrid[player.position.y][player.position.x] = BLANK;
            playGrid[player.position.y + 1][player.position.x] = player.icon;
            player.position.y += 1;
            player.actionPts -= 1;
        }
        else
            cout << "\nDid not move South.\n" << endl;
        break;
    }
}

void turnLeft(Bunny & player)
{
    switch (player.direction)
    {
    case 'N':
    {
        player.direction = 'W';
    }
    break;
    case 'E':
    {
        player.direction = 'N';
    }
    break;
    case 'W':
    {
        player.direction = 'S';
    }
    break;
    case 'S':
    {
        player.direction = 'E';
    }
    break;
    }
    player.actionPts -= 1;
}

void turnRight(Bunny & player)
{
    switch (player.direction)
    {
    case 'N':
    {
        player.direction = 'E';
    }
    break;
    case 'E':
    {
        player.direction = 'S';
    }
    break;
    case 'W':
    {
        player.direction = 'N';
    }
    break;
    case 'S':
    {
        player.direction = 'W';
    }
    break;
    }
    player.actionPts -= 1;
}

void carrotSpawn(Carrot & enemy, unsigned char playGrid[][DIM])
{
    int randRow = rand() % 13 + 1;
    int randCol = rand() % 13 + 1;

    while (playGrid[randRow][randCol] != BLANK) //do not spawn on boarder, items, player, or other enemies
    {
        randRow = rand() % 13 + 1;
        randCol = rand() % 13 + 1;

    }
    enemy.position.x = randCol;
    enemy.position.y = randRow;

    playGrid[enemy.position.y][enemy.position.x] = enemy.icon;
}

void carrotCalc(Bunny & player, Carrot & enemy, unsigned char playGrid[][DIM])
{
    bool stabSuccess = false;
    bool bludgeonSuccess = false;
    int stabChance = rand() % 4 + 1; //25% chance of stabbing

    switch (enemy.direction)
    {
    case 'N':
    {
        if (enemy.position.x == player.position.x && enemy.position.y == (player.position.y - 1))
        {
            if (stabChance == 4)
            {
                cout << "\nCarrot stabs you!\n" << endl;
                player.health -= 2;
                stabSuccess = true;
                player.actionPts -= 3;
            }
            else
                cout << "\nCarrot failed to stab you!" << endl;
        }
    }
    break;
    case 'E':
    {
        if (enemy.position.x == (player.position.x + 1) && enemy.position.y == player.position.y)
        {
             if (stabChance == 4)
            {
                cout << "\nCarrot stabs you!\n" << endl;
                player.health -= 2;
                stabSuccess = true;
                player.actionPts -= 3;
            }
            else
                cout << "\nCarrot failed to stab you!" << endl;
        }

    }
    break;
    case 'W':
    {
        if (enemy.position.x == (player.position.x - 1) && enemy.position.y == player.position.y)
        {
             if (stabChance == 4)
            {
                cout << "\nCarrot stabs you!\n" << endl;
                player.health -= 2;
                stabSuccess = true;
                player.actionPts -= 3;
            }
            else
                cout << "\nCarrot failed to stab you!" << endl;
        }

    }
    break;
    case 'S':
    {
        if (enemy.position.x == player.position.x && enemy.position.y == (player.position.y + 1))
        {
             if (stabChance == 4)
            {
                cout << "\nCarrot stabs you!\n" << endl;
                player.health -= 2;
                stabSuccess = true;
                player.actionPts -= 3;
            }
            else
                cout << "\nCarrot failed to stab you!" << endl;
        }
    }
    break;
    }


    if (stabSuccess == false) //checks each square around
    {
        if (enemy.position.x == player.position.x && enemy.position.y == (player.position.y + 1)) //down
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }

        else if (enemy.position.x == (player.position.x + 1) && enemy.position.y == player.position.y) //right
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
        else if (enemy.position.x == (player.position.x - 1) && enemy.position.y == player.position.y) //left
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
        else if (enemy.position.x == player.position.x && enemy.position.y == (player.position.y - 1)) //up
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
        else if (enemy.position.x == (player.position.x - 1) && enemy.position.y == (player.position.y - 1)) //upper left
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
        else if (enemy.position.x == (player.position.x + 1) && enemy.position.y == (player.position.y - 1)) //upper right
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
        else if (enemy.position.x == (player.position.x - 1) && enemy.position.y == (player.position.y + 1)) //bottom left
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
        else if (enemy.position.x == (player.position.x + 1) && enemy.position.y == (player.position.y + 1)) //bottom right
        {
            cout << "\nCarrot bludgeons you!\n";
            player.health -= 1;
            bludgeonSuccess = true;
        }
    }

    if (stabSuccess == false && bludgeonSuccess == false)
    {
        if (enemy.position.y > player.position.y)
        {
            switch (enemy.direction)
            {
            case 'N':
            {
                if(playGrid[enemy.position.y - 1][enemy.position.x] != enemy.icon) //checks for other enemy
                {
                    playGrid[enemy.position.y][enemy.position.x] = BLANK;
                    playGrid[enemy.position.y - 1][enemy.position.x] = enemy.icon;
                    enemy.position.y -= 1;
                    cout << "\nThe carrot moved forward.\n";
                }
            }
            break;
            case 'W':
            {
                enemy.direction = 'N';
                cout << "\nThe carrot turned right.\n";
            }
            break;
            case 'E':
            {
                enemy.direction = 'N';
                cout << "\nThe carrot turned left.\n";
            }
            break;
            case 'S':
            {
                enemy.direction = 'W';
                cout << "\nThe carrot turned right.\n";
            }
            break;
            }
        }
        else if (enemy.position.y < player.position.y)
        {
            switch (enemy.direction)
            {
            case 'S':
            {
                if(playGrid[enemy.position.y + 1][enemy.position.x] != enemy.icon)
                {
                    playGrid[enemy.position.y][enemy.position.x] = BLANK;
                    playGrid[enemy.position.y + 1][enemy.position.x] = enemy.icon;
                    enemy.position.y += 1;
                    cout << "\nThe carrot moved forward.\n";
                }
            }
            break;
            case 'W':
            {
                enemy.direction = 'S';
                cout << "\nThe carrot turned right.\n";
            }
            break;
            case 'E':
            {
                enemy.direction = 'S';
                cout << "\nThe carrot turned left.\n";
            }
            break;
            case 'N':
            {
                enemy.direction = 'W';
                cout << "\nThe carrot turned left.\n";
            }
            break;
            }
        }
        else if (enemy.position.x < player.position.x)
        {
            switch (enemy.direction)
            {
            case 'E':
            {
                if(playGrid[enemy.position.y][enemy.position.x + 1] != enemy.icon)
                {
                    playGrid[enemy.position.y][enemy.position.x] = BLANK;
                    playGrid[enemy.position.y][enemy.position.x + 1] = enemy.icon;
                    enemy.position.x += 1;
                    cout << "\nThe carrot moved forward.\n";
                }
            }
            break;
            case 'N':
            {
                enemy.direction = 'E';
                cout << "\nThe carrot turned right.\n";
            }
            break;
            case 'S':
            {
                enemy.direction = 'E';
                cout << "\nThe carrot turned left.\n";
            }
            break;
            case 'W':
            {
                enemy.direction = 'N';
                cout << "\nThe carrot turned right.\n";
            }
            break;
            }
        }
        else if (enemy.position.x > player.position.x)
        {
            switch (enemy.direction)
            {
            case 'W':
            {
                if (playGrid[enemy.position.y][enemy.position.x - 1] != enemy.icon)
                {
                    playGrid[enemy.position.y][enemy.position.x] = BLANK;
                    playGrid[enemy.position.y][enemy.position.x - 1] = enemy.icon;
                    enemy.position.x -= 1;
                    cout << "\n The carrot moved forward.\n";
                }
            }
            break;
            case 'N':
            {
                enemy.direction = 'W';
                cout << "\nThe carrot turned left.\n";
            }
            break;
            case 'S':
            {
                enemy.direction = 'W';
                cout << "\nThe carrot turned right.\n";
            }
            break;
            case 'E':
            {
                enemy.direction = 'N';
                cout << "\nThe carrot turned left.\n";
            }
            break;
            }
        }
    }
    enemy.actionPts -= 1;
}

void bunnyBite(Bunny & player, Carrot & enemyA, Carrot & enemyB, unsigned char playGrid[][DIM])
{
    switch (player.direction)
    {
    case 'N':
    {
        if (player.position.x == enemyA.position.x && (player.position.y - 1) == enemyA.position.y)
        {
            biteCalc(player, enemyA);
        }
        else if (player.position.x == enemyB.position.x && (player.position.y - 1) == enemyB.position.y)
        {
            biteCalc(player, enemyB);
        }
    }
    break;
    case 'W':
    {
        if ((player.position.x - 1) == enemyA.position.x && player.position.y == enemyA.position.y)
        {
            biteCalc(player, enemyA);
        }
        else if ((player.position.x - 1) == enemyB.position.x && player.position.y == enemyB.position.y)
        {
            biteCalc(player, enemyB);
        }
    }
    break;
    case 'E':
    {
        if ((player.position.x + 1) == enemyA.position.x && player.position.y == enemyA.position.y)
        {
            biteCalc(player, enemyA);
        }
        else if ((player.position.x + 1) == enemyB.position.x && player.position.y == enemyB.position.y)
        {
            biteCalc(player, enemyB);
        }
    }
    break;
    case 'S':
    {
        if (player.position.x == enemyA.position.x && (player.position.y + 1) == enemyA.position.y)
        {
            biteCalc(player, enemyA);
        }
        else if (player.position.x == enemyB.position.x && (player.position.y + 1) == enemyB.position.y)
        {
            biteCalc(player, enemyB);
        }
        break;
    }

    }

}

void biteCalc(Bunny & player, Carrot & enemy)
{
    enemy.health -= 1;
    player.biteCount -= 1;
    player.actionPts -= 1;
    cout << "\nYou bit the carrot!" << endl;
}

void itemSpawn(unsigned char playGrid[][DIM])
{
    int chooseItem = rand() % 3 + 1; //33% chance of each item appearing

    if (chooseItem == 1)
    {
        cout << "Gun has spawned on the board!" << endl;

        int gunRow = rand() % 13 + 1;
        int gunCol = rand() % 13 + 1;

        while (playGrid[gunRow][gunCol] != BLANK)
        {
            gunRow = rand() % 13 + 1;
            gunCol = rand() % 13 + 1;
        }
        playGrid[gunRow][gunCol] = 'G';
    }
    else if (chooseItem == 2)
    {
        cout << "Pellet has spawned on the board!" << endl;

        int pelRow = rand() % 13 + 1;
        int pelCol = rand() % 13 + 1;

        while (playGrid[pelRow][pelCol] != BLANK)
        {
            pelRow = rand() % 13 + 1;
            pelCol = rand() % 13 + 1;
        }
        playGrid[pelRow][pelCol] = 'O';
    }
    else
    {
        cout << "Food has spawned on the board!" << endl;
        int foodRow = rand() % 13 + 1;
        int foodCol = rand() % 13 + 1;

        while (playGrid[foodRow][foodCol] != BLANK)
        {
            foodRow = rand() % 13 + 1;
            foodCol = rand() % 13 + 1;
        }

        playGrid[foodRow][foodCol] = 'F';
    }
}

void pickUpItem(unsigned char playGrid[][DIM], Bunny & player)
{
    char item = ' ';

    switch (player.direction)
    {
    case 'N':
    {
        if (playGrid[player.position.y - 1][player.position.x] == 'O')
        {
            item = 'O';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y - 1][player.position.x] = BLANK;
        }
        else if (playGrid[player.position.y - 1][player.position.x] == 'G')
        {
            item = 'G';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y - 1][player.position.x] = BLANK;
        }
        else if (playGrid[player.position.y - 1][player.position.x] == 'F')
        {
            item = 'F';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y - 1][player.position.x] = BLANK;
        }
    }
    break;
    case 'W':
    {
        if (playGrid[player.position.y][player.position.x - 1] == 'O')
        {
            item = 'O';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y][player.position.x - 1] = BLANK;
        }
        else if (playGrid[player.position.y][player.position.x - 1] == 'G')
        {
            item = 'G';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y][player.position.x - 1] = BLANK;
        }
        else if (playGrid[player.position.y][player.position.x - 1] == 'F')
        {
            item = 'F';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y][player.position.x - 1] = BLANK;
        }
    }
    break;
    case 'E':
    {
        if (playGrid[player.position.y][player.position.x + 1] == 'O')
        {
            item = 'O';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y][player.position.x + 1] = BLANK;
        }
        else if (playGrid[player.position.y][player.position.x + 1] == 'G')
        {
            item = 'G';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y][player.position.x + 1] = BLANK;
        }
        else if (playGrid[player.position.y][player.position.x + 1] == 'F')
        {
            item = 'F';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y][player.position.x + 1] = BLANK;
        }
    }
    break;
    case 'S':
    {
        if (playGrid[player.position.y + 1][player.position.x] == 'O')
        {
            item = 'O';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y + 1][player.position.x] = BLANK;
        }
        else if (playGrid[player.position.y + 1][player.position.x] == 'G')
        {
            item = 'G';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y + 1][player.position.x] = BLANK;
        }
        else if (playGrid[player.position.y + 1][player.position.x] == 'F')
        {
            item = 'F';
            itemCalc(playGrid, player, item);
            playGrid[player.position.y + 1][player.position.x] = BLANK;
        }
    }
    break;
    }
}

void itemCalc(unsigned char playGrid[][DIM], Bunny & player, char item)
{
    int foodChance = rand() % 2 + 1; //50% chance of food having 1 or 2 meals


    if (item == 'O')
    {
        cout << "\nYou've picked up a pellet! You now have 3 more bites!\n";
        player.pelletCount += 1;
        player.biteCount += 3;
        player.actionPts -= 1;
        player.withoutPellet = 0; //reset withoutPellet and avoid a game over
    }
    else if (item == 'G')
    {
        cout << "\nYou've picked up a gun!\n";
        player.haveGun = true;
        player.actionPts -= 1;
    }
    else if (item == 'F')
    {
        cout << "\nYou've picked up some food!";

        if (foodChance == 1)
        {
            player.health += 5;
            cout << "It has one meal! (+5 health)\n";
        }
        else
        {
            player.health += 10;
            cout << "It has two meals! (+10 health)\n";
        }
        player.actionPts -= 1;
    }

}

void shootCalc(unsigned char playGrid[][DIM], Bunny & player, Carrot & enemyA, Carrot & enemyB)
{
    switch (player.direction)
    {
    case 'N':
    {
        if (player.position.x == enemyA.position.x && player.position.y > enemyA.position.y) //in the same column as enemy and below enemy
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyA.health -= 1;
            player.actionPts -= 1;
        }
        else if (player.position.x == enemyB.position.x && player.position.y > enemyB.position.y) //enemyB
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyB.health -= 1;
            player.actionPts -= 1;
        }
    }
    break;
    case 'W':
    {
        if (player.position.x > enemyA.position.x && player.position.y == enemyA.position.y)
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyA.health -= 1;
            player.actionPts -= 1;
        }
        else if (player.position.x > enemyB.position.x && player.position.y == enemyB.position.y)
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyB.health -= 1;
            player.actionPts -= 1;
        }
    }
    break;
    case 'E':
    {
        if (player.position.x < enemyA.position.x && player.position.y == enemyA.position.y)
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyA.health -= 1;
            player.actionPts -= 1;
        }
        else if (player.position.x < enemyB.position.x && player.position.y == enemyB.position.y)
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyB.health -= 1;
            player.actionPts -= 1;
        }
    }
    break;
    case 'S':
    {
        if (player.position.x == enemyA.position.x && player.position.y < enemyA.position.y)
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyA.health -= 1;
            player.actionPts -= 1;
        }
        else if (player.position.x == enemyB.position.x && player.position.y < enemyB.position.y)
        {
            cout << "\nYou shot the enemy!" << endl;
            enemyB.health -= 1;
            player.actionPts -= 1;
        }
    }
    break;
    }
}

