#include "hull.h"
#include <stdio.h>

HEADER *header;

int planes_num;
PLANE *planes;

int clipnodes_num;
CLIPNODE *clipnodes;

int models_num;
MODEL *models;

char *bsp;
int size;

int main(int argc, char ** argv)
{
    if(argc != 2) return 1;

    FILE *fp = fopen(argv[1], "rb");
    if(!fp) return 2;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    bsp = (char*)malloc(size);
    if(!bsp) return 3;
    if(1 != fread(bsp, size, 1, fp)) return 4;
    fclose(fp);

    header = (HEADER*)bsp;
    if(header->version != 30) return 5;

    planes_num = header->lump[LUMP_PLANES].size / sizeof(PLANE);
    planes = (PLANE*)&bsp[header->lump[LUMP_PLANES].pos];

    clipnodes_num = header->lump[LUMP_CLIPNODES].size / sizeof(CLIPNODE);
    clipnodes = (CLIPNODE*)&bsp[header->lump[LUMP_CLIPNODES].pos];

    models_num = header->lump[LUMP_MODELS].size / sizeof(MODEL);
    models = (MODEL*)&bsp[header->lump[LUMP_MODELS].pos];

    printf("map: %s\n", argv[1]);
    printf("models: %d\n", models_num);
    printf("planes: %d\n", planes_num);
    printf("clipnodes: %d\n", clipnodes_num);

    
    proc();

    printf("Holes: %ld\n", empty_tris[0].size()+empty_tris[1].size()+empty_tris[2].size());

    view(argc, argv);
    return 0;
}