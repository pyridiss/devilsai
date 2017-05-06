addImageArchiveFile img/Zombie.img
MATRICE_TEINTE 0.2 0.2 0.2 0.2 0.3 0.2 0.4 0.4 0.9
DIPLOM 2
CMP_VIS 450
RAY_COL 14
RAY_INTER 30
ACT 6 blo 3
VIT 8
MAJ 0
DIR 1 numberOfImages 9 pathToImages 6/1/%%.png
FIN_ACT
ACT 5 blo 2
VIT 14
MAJ 0
DIR 0 numberOfImages 8 pathToImages 5/0/%%.png
DIR 1 numberOfImages 8 pathToImages 5/1/%%.png
DIR 2 numberOfImages 8 pathToImages 5/2/%%.png
DIR 3 numberOfImages 8 pathToImages 5/3/%%.png
FIN_ACT
ACT 4 blo 1
VIT 10
MAJ 0
DIR 0 numberOfImages 10 pathToImages 4/0/%%.png
DIR 1 numberOfImages 10 pathToImages 4/1/%%.png
DIR 2 numberOfImages 10 pathToImages 4/2/%%.png
DIR 3 numberOfImages 10 pathToImages 4/3/%%.png
FIN_ACT
ACT 2 blo 1
VIT 5
MAJ 0
DIR 0 numberOfImages 8 pathToImages 2/0/%%.png
DIR 1 numberOfImages 8 pathToImages 2/1/%%.png
DIR 2 numberOfImages 8 pathToImages 2/2/%%.png
DIR 3 numberOfImages 8 pathToImages 2/3/%%.png
FIN_ACT
ACT 1 blo 0
VIT 7
MAJ 3
DIR 0 numberOfImages 8 pathToImages 1/0/%%.png
DIR 1 numberOfImages 8 pathToImages 1/1/%%.png
DIR 2 numberOfImages 8 pathToImages 1/2/%%.png
DIR 3 numberOfImages 8 pathToImages 1/3/%%.png
FIN_ACT
ACT_DEFAUT 1
FORCE 270
PUISS 100
AGILITE 65
INTELLI 15
CONSTIT 70
ESQUIVE 10
CHARISM 40
RECUP 28
EXP 135
