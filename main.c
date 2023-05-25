#include "DogeLib/Includes.h"
#define EVIL(EVILR, EVILG, EVILB) {.r = 0x##EVILR, .g = 0x##EVILG, .b = 0x##EVILB, .a = 255}
#define PCE(E0,E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12,E13,E14,E15) (E0<<15|E1<<14|E2<<13|E3<<12|E4<<11|E5<<10|E6<<9|E7<<8|E8<<7|E9<<6|E10<<5|E11<<4|E12<<3|E13<<2|E14<<1|E15)
#define PAT(P, X, Y) ((P)>>(15-(4*(Y)+(X)))&1)
#define PST(S, X, Y) (S<<(4*(Y)+(X)))
const Length gridLen = {.x = 12, .y = 21};

typedef enum{T_EMPTY, T_GREY, T_BLUE, T_GREEN, T_MAGENTA, T_ORANGE, T_RED, T_CYAN, T_YELLOW, T_NUM}Tile;
const Color TileColor[T_NUM][5] = {
    {BLACK, BLACK, BLACK, BLACK, BLACK},
    {EVIL(5b,59,5a), EVIL(9e,9c,9d), EVIL(7c,7c,7c), EVIL(3c,3a,3b), EVIL(2a,27,29)},
    {EVIL(01,01,f6), EVIL(67,66,ff), EVIL(36,33,ff), EVIL(00,00,a9), EVIL(00,00,73)},
    {EVIL(ff,00,fd), EVIL(ff,68,fe), EVIL(fd,36,fd), EVIL(a7,01,ab), EVIL(75,00,75)},
    {EVIL(ff,7d,02), EVIL(ff,b2,69), EVIL(fe,96,35), EVIL(a9,52,04), EVIL(75,39,04)},
    {EVIL(ff,00,00), EVIL(ff,69,65), EVIL(fe,33,36), EVIL(ab,00,03), EVIL(76,00,04)},
    {EVIL(00,ff,02), EVIL(65,ff,66), EVIL(33,ff,35), EVIL(00,a7,01), EVIL(03,74,02)},
    {EVIL(ff,fd,05), EVIL(fe,fe,67), EVIL(ff,ff,37), EVIL(a8,a6,03), EVIL(73,76,03)}
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

Length pieceOffset(const u16 piece)
{
    Coord start = {.x=4,.y=4};
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(PAT(piece,x,y)){
                start.x = imin(start.x, x);
                start.y = imin(start.y, y);
            }
        }
    }
    return start;
}

Length pieceBounds(const u16 piece)
{
    Coord last = {0};
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(PAT(piece,x,y)){
                last.x = imax(last.x, x);
                last.y = imax(last.y, y);
            }
        }
    }
    return coordMost((const Coord){0}, coordSub(coordAddi(last, 1), pieceOffset(piece)));
}

u16 pieceShift(const u16 piece)
{
    Coord start = {.x=4,.y=4};
    Coord last = {0};
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(PAT(piece,x,y)){
                start.x = imin(start.x, x);
                start.y = imin(start.y, y);
                last.x = imax(last.x, x);
                last.y = imax(last.y, y);
            }
        }
    }
    last = coordMost((const Coord){0}, coordSub(coordAddi(last, 1), start));
    u16 shifted = 0;
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            u16 t = PAT(piece, x+start.x, y+start.y);
            shifted |= PST(t, x, y);
        }
    }
    return shifted;
}

void piecePrint(const u16 p)
{
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            printf("%c", PAT(p, x, y)?'#':'-');
        }
        printf("\n");
    }
    const Length pb = pieceBounds(p);
    printf("(%i,%i)\n", pb.x, pb.y);
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

u16 pieceRor(const u16 p)
{
    const Length l = pieceBounds(p);
    if(l.x == 2 && l.y == 2)
        return p;

    u16 r = 0;
    if(coordMax(l) == 4){
// - X -     - # -
// # # #     - # X
// - - -     - # -
// 1,0        2,1
    }else{
        for(int y = 0; y < 4; y++){
            for(int x = 0; x < 4; x++){
                    r |= PST(PAT(p, x, y), 3-y, x);
            }
        }
    }
    return r;
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

    u16 p = piece[4];
    for(int i = 0; i < 4; i++){
        const Coord b = pieceBounds(p);
        printf("off (%i,%i)\n", b.x, b.y);
        piecePrint(p);
        p = pieceShift(pieceRor(p));
    }
    while(1){
        const uint t = frameStart();

        const int scale = gridGetScale();
        const Coord off = gridGetOffset();
        if(keyPressed(SDL_SCANCODE_ESCAPE)){
            gridFree(grid);
            exit(EXIT_SUCCESS);
        }

        gridDraw(grid, off, scale);
        u16 p = piece[4];
        for(int i = 0; i < 4; i++){
            const Coord pos = (const Coord){.x=2, .y=i*4};
            pieceDraw(pos, p, 4);
            setColor(WHITE);
            const Coord b = pieceBounds(p);
            drawRectCoordLength(coordAdd(coordMuli(pos,scale), off), coordMuli(b,scale));
            p = pieceShift(pieceRor(p));
        }
        p = piece[4];
        for(int i = 0; i < 4; i++){
            const Coord pos = (const Coord){.x=7, .y=i*4};
            pieceDraw(pos, p, 5);
            setColor(WHITE);
            drawRectCoordLength(coordAdd(coordMuli(pos,scale), off), coordMuli(pieceBounds(p),scale));
            p = pieceRor(p);
        }

        frameEnd(t);
    }
    return 0;
}
