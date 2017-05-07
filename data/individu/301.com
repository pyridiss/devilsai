addImageArchiveFile img/Araignee.img
MATRICE_TEINTE 0 0 0.8 0 0.8 0 1.1 0 0
corpseImageKey 301:6/1/09.png
DIPLOM 2
CMP_VIS 450
RAY_COL 14
RAY_INTER 30
STUFF spidervenom 1 10
ACT 6 blo 3
VIT 9
MAJ 0
DIR 1 numberOfImages 10 pathToImages 6/1/%%.png
FIN_ACT
ACT 5 blo 2
VIT 20
MAJ 0
DIR 0 numberOfImages 8 pathToImages 5/0/%%.png
DIR 1 numberOfImages 8 pathToImages 5/1/%%.png
DIR 2 numberOfImages 8 pathToImages 5/2/%%.png
DIR 3 numberOfImages 8 pathToImages 5/3/%%.png
FIN_ACT
ACT 4 blo 1
VIT 9
MAJ 0
DIR 0 numberOfImages 8 pathToImages 4/0/%%.png
DIR 1 numberOfImages 8 pathToImages 4/1/%%.png
DIR 2 numberOfImages 8 pathToImages 4/2/%%.png
DIR 3 numberOfImages 8 pathToImages 4/3/%%.png
FIN_ACT
ACT 2 blo 1
VIT 7
MAJ 0
DIR 1 numberOfImages 7 pathToImages 2/1/%%.png
FIN_ACT
ACT 1 blo 0
VIT 12
MAJ 2
DIR 0 numberOfImages 8 pathToImages 1/0/%%.png
DIR 1 numberOfImages 8 pathToImages 1/1/%%.png
DIR 2 numberOfImages 8 pathToImages 1/2/%%.png
DIR 3 numberOfImages 8 pathToImages 1/3/%%.png
FIN_ACT
ACT_DEFAUT 1
FORCE 125
PUISS 60
AGILITE 46
INTELLI 24
CONSTIT 30
ESQUIVE 20
CHARISM 32
RECUP 55
EXP 55
