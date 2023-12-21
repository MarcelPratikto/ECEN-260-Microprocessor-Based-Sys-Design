#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 84
#define HEIGHT 48
#define NUM_BANKS 6

char IMAGE[HEIGHT][WIDTH];
char SCREEN[WIDTH][NUM_BANKS];

struct Player{
	int x;
	int y;
	char* model;
	int model_width;
	int model_height;
};
char player_model[15] = {
	"  x  "
	" xxx "
	"xxxxx"
};

void convertToImage(struct Player player)
{
    int player_model_pos = 0;
	// create IMAGE
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			if (x >= player.x && x < player.x+player.model_width && y >= player.y && y < player.y+player.model_height)
			{
                IMAGE[y][x] = player.model[player_model_pos];
                ++player_model_pos;
			}
            else
            {
                IMAGE[y][x] = ' ';
            }
            // if (IMAGE[y][x] == 'x')
            //     printf("IMAGE[%d][%d] == %c\n", y, x, IMAGE[y][x]);
		}
        //printf("image[%d]: %s\n", y, IMAGE[y] + '\0');
	}
}

void convertToScreen()
{
	// convert IMAGE to SCREEN
	for (int x = 0; x < WIDTH; ++x)
	{
        int bank = 0;
        int pixel = 0;
        char binary = 0b00000000;
        for (int y = 0; y < HEIGHT; ++y)
        {
            if (IMAGE[y][x] == 'x')
            {
                binary |= (1 << pixel);
            }
            ++pixel;
            if (pixel == 8)
            {
                SCREEN[x][bank] = binary;
                // if (SCREEN[x][bank] != 0)
                //     printf("SCREEN[%d][%d]: %x\n", x, bank, SCREEN[x][bank]);
                ++bank;
                pixel = 0;
                binary = 0b00000000;
            }
        }
	}
}

void drawScreen()
{
	for (int y=0; y<6; ++y)
	{
		for (int x=0; x<84; ++x)
		{
            printf("SCREEN[%d][%d]: %d\n", x, y, SCREEN[x][y]);
		  //GLCD_data_write(SCREEN[x][y]);
		}
	}
	//HAL_Delay(5000);
	//GLCD_clear();
}

int main(void)
{
    int end = 0;
    while (!end)
    {
        struct Player player;
        player.x = 0;
        player.y = 0;
        player.model = &player_model;
        player.model_width = 5;
        player.model_height = 3;

        convertToImage(player);
        convertToScreen();
        drawScreen();

        printf("TEST: %d", 0b10000000);
        printf("\nEND PROGRAM? ");
        char userInput[3];
        scanf("%s", &userInput);
        if (strcmp(userInput, "END") == 0)
            end = 1;
    }
}