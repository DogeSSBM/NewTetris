#include "DogeLib/Includes.h"
#define RGB(R, G, B) {.r = 0x##R, .g = 0x##G, .b = 0x##B, .a = 255}
#define PCE(E0,E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12,E13,E14,E15) (E0<<15|E1<<14|E2<<13|E3<<12|E4<<11|E5<<10|E6<<9|E7<<8|E8<<7|E9<<6|E10<<5|E11<<4|E12<<3|E13<<2|E14<<1|E15)
#define PAT(P, X, Y) ((P)>>(15-(4*(Y)+(X)))&1)
#define PST(S, X, Y) (S<<(4*(Y)+(X)))
const Length gridLen = {.x = 12, .y = 21};

typedef enum{T_EMPTY, T_GREY, T_BLUE, T_GREEN, T_MAGENTA, T_ORANGE, T_RED, T_CYAN, T_YELLOW, T_NUM}Tile;
const Color TileColor[T_NUM][5] = {
    {BLACK, BLACK, BLACK, BLACK, BLACK},
    {RGB(5b,59,5a), RGB(9e,9c,9d), RGB(7c,7c,7c), RGB(3c,3a,3b), RGB(2a,27,29)},
    {RGB(01,01,f6), RGB(67,66,ff), RGB(36,33,ff), RGB(00,00,a9), RGB(00,00,73)},
    {RGB(ff,00,fd), RGB(ff,68,fe), RGB(fd,36,fd), RGB(a7,01,ab), RGB(75,00,75)},
    {RGB(ff,7d,02), RGB(ff,b2,69), RGB(fe,96,35), RGB(a9,52,04), RGB(75,39,04)},
    {RGB(ff,00,00), RGB(ff,69,65), RGB(fe,33,36), RGB(ab,00,03), RGB(76,00,04)},
    {RGB(00,ff,02), RGB(65,ff,66), RGB(33,ff,35), RGB(00,a7,01), RGB(03,74,02)},
    {RGB(ff,fd,05), RGB(fe,fe,67), RGB(ff,ff,37), RGB(a8,a6,03), RGB(73,76,03)}
};

const u16 piece[T_NUM] = {
    0,
    PCE(1,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0),
    PCE(1,0,0,0,
        1,1,1,0,
        0,0,0,0,
        0,0,0,0),
    PCE(0,1,1,0,
        1,1,0,0,
        0,0,0,0,
        0,0,0,0),
    PCE(0,1,0,0,
        1,1,1,0,
        0,0,0,0,
        0,0,0,0),
    PCE(0,0,1,0,
        1,1,1,0,
        0,0,0,0,
        0,0,0,0),
    PCE(1,1,0,0,
        0,1,1,0,
        0,0,0,0,
        0,0,0,0),
    PCE(0,0,0,0,
        1,1,1,1,
        0,0,0,0,
        0,0,0,0),
    PCE(0,1,1,0,
        0,1,1,0,
        0,0,0,0,
        0,0,0,0),
};

void piecePrint(const u16 p)
{
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            printf("%c", PAT(p, x, y)?'#':'-');
        }
        printf("\n");
    }
    printf("\n");
}

int gridGetScale(void)
{
    return coordMin(coordDiv(getWindowLen(), gridLen));
}

Coord gridGetOffset(void)
{
    const Coord wlen = getWindowLen();
    return coordDivi(coordSub(wlen, coordMuli(gridLen, coordMin(coordDiv(wlen, gridLen)))), 2);
}

void tileDraw(const Coord spos, const Tile tile, const int scale)
{
    if(!tile)
        return;
    const int qscale = scale/4;
    const Coord corners[4] = {
        spos,
        {.x=spos.x+scale-1, .y=spos.y},
        {.x=spos.x+scale-1, .y=spos.y+scale-1},
        {.x=spos.x, .y=spos.y+scale-1}
    };

    for(Direction d = 0; d < 4; d++){
        for(int i = 0; i < qscale; i++){
            setColor(TileColor[tile][1+d]);
            const Coord c = coordShift(coordShift(corners[d], dirINV(d), i), dirROR(d), i);
            drawLineCoords(c, coordShift(c, dirROR(d), (scale-i*2)-1-dirPOS(dirROL(d))));
        }
    }
    setColor(TileColor[tile][0]);
    fillSquareCoord(coordAddi(spos, qscale), scale/2+1);
}

void pieceDraw(const Coord pos, const u16 piece, const Tile tile)
{
    const int scale = gridGetScale();
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            const Coord psoff = coordMuli((const Coord){.x=x,.y=y}, scale);
            const Coord ppos = coordAdd(coordMuli(pos, scale), gridGetOffset());
            if(PAT(piece, x, y)){
                tileDraw(coordAdd(psoff, ppos), tile, scale);
            }
        }
    }
}

void gridDraw(Tile **grid, const Coord off, const int scale)
{
    setColor(GREY);
    for(int x = 0; x < gridLen.x; x++)
        for(int y = 0; y < gridLen.y; y++)
            tileDraw(coordAdd(coordMuli(iC(x,y), scale), off), grid[x][y], scale);
}

Tile** gridInit(void)
{
    Tile **grid = calloc(gridLen.x, sizeof(Tile*));
    for(int i = 0; i < gridLen.x; i++)
        grid[i] = calloc(gridLen.y, sizeof(Tile));

    for(int x = 0; x < gridLen.x; x++)
        for(int y = 0; y < gridLen.y; y++)
            if(x == 0 || x == gridLen.x-1 || y == gridLen.y-1)
                grid[x][y] = T_GREY;

    return grid;
}

void gridFree(Tile **grid)
{
    if(!grid)
        return;
    for(int i = 0; i < gridLen.x; i++)
        if(grid[i])
            free(grid[i]);
    free(grid);
}

int main(void)
{
    init();
    for(int i = 0; i < T_NUM; i++)
        piecePrint(piece[i]);

    Tile **grid = gridInit();
    while(1){
        const uint t = frameStart();

        const int scale = gridGetScale();
        const Coord off = gridGetOffset();
        if(keyPressed(SDL_SCANCODE_ESCAPE)){
            gridFree(grid);
            exit(EXIT_SUCCESS);
        }

        gridDraw(grid, off, scale);

        frameEnd(t);
    }
    return 0;
}
