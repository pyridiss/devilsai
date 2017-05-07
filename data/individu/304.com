addImageArchiveFile img/monster-spider-greyscale.img
changeHSL 0 100 50
corpseImageKey 304:death/south/09.png
DIPLOM 2
CMP_VIS 450
RAY_COL 14
RAY_INTER 30
STUFF spidervenom 1 10
ACT 6 blo 3
VIT 9
MAJ 0
DIR 1 numberOfImages 10 pathToImages death/south/%%.png
FIN_ACT
ACT 5 blo 2
VIT 20
MAJ 0
DIR 0 numberOfImages 8 pathToImages hurt/north/%%.png
DIR 1 numberOfImages 8 pathToImages hurt/south/%%.png
DIR 2 numberOfImages 8 pathToImages hurt/east/%%.png
DIR 3 numberOfImages 8 pathToImages hurt/west/%%.png
FIN_ACT
ACT 4 blo 1
VIT 14
MAJ 0
DIR 0 numberOfImages 8 pathToImages attack/north/%%.png
DIR 1 numberOfImages 8 pathToImages attack/south/%%.png
DIR 2 numberOfImages 8 pathToImages attack/east/%%.png
DIR 3 numberOfImages 8 pathToImages attack/west/%%.png
FIN_ACT
ACT 2 blo 1
VIT 7
MAJ 0
DIR 1 numberOfImages 7 pathToImages pause/south/%%.png
FIN_ACT
ACT 1 blo 0
VIT 18
MAJ 2
DIR 0 numberOfImages 8 pathToImages walking/north/%%.png
DIR 1 numberOfImages 8 pathToImages walking/south/%%.png
DIR 2 numberOfImages 8 pathToImages walking/east/%%.png
DIR 3 numberOfImages 8 pathToImages walking/west/%%.png
FIN_ACT
ACT_DEFAUT 1
FORCE 265
PUISS 140
AGILITE 105
INTELLI 50
CONSTIT 80
ESQUIVE 45
CHARISM 45
RECUP 72
EXP 200
