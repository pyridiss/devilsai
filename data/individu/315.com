addImageArchiveFile img/Ogre.img
corpseImageKey 315:6/1/10.png
DIPLOM 2
CMP_VIS 450
RAY_COL 17
RAY_INTER 35
STUFF healpotion 1 5
STUFF attackpotion 1 5
STUFF defensepotion 1 5
STUFF speedpotion 1 5
ACT 6 blo 3
VIT 8
MAJ 0
angle 90 numberOfImages 11 pathToImages 6/1/%%.png
FIN_ACT
ACT 5 blo 2
VIT 11
MAJ 0
angle 270 numberOfImages 8 pathToImages 5/0/%%.png
angle 90 numberOfImages 8 pathToImages 5/1/%%.png
angle 0 numberOfImages 8 pathToImages 5/2/%%.png
angle 180 numberOfImages 8 pathToImages 5/3/%%.png
FIN_ACT
ACT 4 blo 1
VIT 9
MAJ 0
angle 270 numberOfImages 10 pathToImages 4/0/%%.png
angle 90 numberOfImages 10 pathToImages 4/1/%%.png
angle 0 numberOfImages 10 pathToImages 4/2/%%.png
angle 180 numberOfImages 10 pathToImages 4/3/%%.png
FIN_ACT
ACT 3 blo 0
VIT 13
MAJ 7
angle 270 numberOfImages 8 pathToImages 3/0/%%.png
angle 90 numberOfImages 8 pathToImages 3/1/%%.png
angle 0 numberOfImages 8 pathToImages 3/2/%%.png
angle 180 numberOfImages 8 pathToImages 3/3/%%.png
FIN_ACT
ACT 2 blo 1
VIT 5
MAJ 0
angle 270 numberOfImages 6 pathToImages 2/0/%%.png
angle 90 numberOfImages 6 pathToImages 2/1/%%.png
angle 0 numberOfImages 6 pathToImages 2/2/%%.png
angle 180 numberOfImages 6 pathToImages 2/3/%%.png
FIN_ACT
ACT_DEFAUT 3
FORCE 165
PUISS 60
AGILITE 55
INTELLI 10
CONSTIT 40
ESQUIVE 5
CHARISM 40
RECUP 20
EXP 85
