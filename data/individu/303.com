addImageArchiveFile img/Araignee.img
MATRICE_TEINTE 0.7 0.4 0 0.7 0.4 0 0.1 0.1 0.1
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
VIT 10
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
VIT 16
MAJ 2
DIR 0 numberOfImages 8 pathToImages 1/0/%%.png
DIR 1 numberOfImages 8 pathToImages 1/1/%%.png
DIR 2 numberOfImages 8 pathToImages 1/2/%%.png
DIR 3 numberOfImages 8 pathToImages 1/3/%%.png
FIN_ACT
ACT_DEFAUT 1
FORCE 195
PUISS 90
AGILITE 70
INTELLI 38
CONSTIT 55
ESQUIVE 30
CHARISM 38
RECUP 60
EXP 135
