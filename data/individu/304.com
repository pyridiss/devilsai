addImageArchiveFile img/Araignee.img
MATRICE_TEINTE 1.1 0 0 0 0 0 0 0 0
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
VIT 14
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
VIT 18
MAJ 2
DIR 0 numberOfImages 8 pathToImages 1/0/%%.png
DIR 1 numberOfImages 8 pathToImages 1/1/%%.png
DIR 2 numberOfImages 8 pathToImages 1/2/%%.png
DIR 3 numberOfImages 8 pathToImages 1/3/%%.png
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
