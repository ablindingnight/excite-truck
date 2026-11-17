#ifndef GAME_PHOBNODE_H
#define GAME_PHOBNODE_H
// The physics-object grid (phoblist.cpp): each cell heads a list of nodes, one per object in it; physcol walks
// the cells around a position. The objects are collision volumes. Names unproven.

class CollVolume;

struct PhobNode {
    PhobNode(void) {
        phob = 0;
        next = 0;
    }
    CollVolume* phob;  // offset 0x0
    PhobNode* next;    // offset 0x4
};

struct PhobCell {
    PhobCell(void) { head = 0; }
    PhobNode* head;  // offset 0x0
};

#endif
