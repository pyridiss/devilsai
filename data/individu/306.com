addImageArchiveFile img/Orc.img
MATRICE_TEINTE 0.8 0 0 0 0 0.8 0 1.2 0
corpseImageKey 306:6/1/12.png
DIPLOM 2
CMP_VIS 400
RAY_COL 24
RAY_INTER 45
STUFF healpotion 1 10
STUFF attackpotion 1 5
STUFF defensepotion 1 5
STUFF speedpotion 1 5
ACT 6 blo 3
VIT 8
MAJ 0
angle 90 numberOfImages 13 pathToImages 6/1/%%.png
FIN_ACT
ACT 5 blo 2
VIT 18
MAJ 0
angle 270 numberOfImages 8 pathToImages 5/0/%%.png
angle 90 numberOfImages 8 pathToImages 5/1/%%.png
angle 0 numberOfImages 8 pathToImages 5/2/%%.png
angle 180 numberOfImages 8 pathToImages 5/3/%%.png
FIN_ACT
ACT 4 blo 1
VIT 9
MAJ 0
angle 270 numberOfImages 12 pathToImages 4/0/%%.png
angle 90 numberOfImages 12 pathToImages 4/1/%%.png
angle 0 numberOfImages 12 pathToImages 4/2/%%.png
angle 180 numberOfImages 12 pathToImages 4/3/%%.png
FIN_ACT
ACT 2 blo 1
VIT 6
MAJ 0
angle 90 numberOfImages 8 pathToImages 2/1/%%.png
FIN_ACT
ACT 1 blo 0
VIT 10
MAJ 3
angle 270 numberOfImages 8 pathToImages 1/0/%%.png
angle 90 numberOfImages 8 pathToImages 1/1/%%.png
angle 0 numberOfImages 8 pathToImages 1/2/%%.png
angle 180 numberOfImages 8 pathToImages 1/3/%%.png
FIN_ACT
ACT_DEFAUT 1
FORCE 215
PUISS 75
AGILITE 46
INTELLI 15
CONSTIT 44
ESQUIVE 8
CHARISM 38
RECUP 22
EXP 60
