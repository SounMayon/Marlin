# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris)

## Références figées

- Cible : MarlinFirmware/Marlin tag `2.1.2.9`, commit `7d300b42acadb3e7467e9f4b4fc03ebea592c2d0`.
- Source constructeur : SounMayon/Mizar-S, branche `main` (fork de Geeetech3D/Mizar-S ; le contenu doit encore être comparé avec le dépôt amont et la build rapportée sur l'imprimante).
- Branche de travail : `mizar-s-marlin-2.1.2.9`.
- Les branches `main` et `bugfix-2.1.x` ne sont pas modifiées.

## Commits présents (sauvegarde GitHub)

- `83a15f1` — début de portage Configuration.h : carte, port série, dimensions 255×255×260, fins de course, paramètres cinématiques, BLTouch, fonctions MBL/ABL, EEPROM/SD/TFT.
- `c42753d` — Z dual, reprise après coupure, pause avancée et options de capteur.
- `46520a3` — affectation PA15 au capteur filament natif (à contrôler avec la sélection de l'écran MKS).
- `b636a8a` — activation du second pilote Z.

Chaque modification de fichier a été enregistrée par un commit GitHub. Ce journal doit être actualisé après chaque grand bloc fonctionnel.

## Architecture de nivellement retrouvée dans le firmware Geeetech

Le constructeur ne se contente pas de définir simultanément les deux macros. Il a séparé les données MBL du singleton bilinéaire :
- l'objet ABL bilinéaire conserve le nom `bedlevel` ;
- l'objet MBL est nommé `mbl` dans `feature/bedlevel/mbl/mesh_bed_leveling.cpp` ;
- le choix runtime est piloté par `auto_manu_level_sel` ;
- `bedlevel.cpp` et `planner.cpp` appliquent le correctif actif selon ce sélecteur ;
- le traitement MBL G29 classique est désactivé et le G29 ABL de Geeetech route les appels entre ABL et MBL ;
- le MBL a des fonctions spécifiques de sauvegarde/restauration et d'interface LVGL.

C'est ce modèle qu'il faut porter, en adaptant les API au Marlin 2.1.2.9. Il ne faut pas enlever seulement le sanity check « select only one leveling option ».

## État réel — non validé matériellement

- La branche comporte un premier jeu de paramètres Mizar, mais ceux-ci sont encore préliminaires.
- Le portage complet du moteur double MBL/ABL n'est pas fini.
- Le parcours de stockage EEPROM des deux grilles est à réconcilier avec le format natif 2.1.2.9 ; le format EEPROM doit être revu/versionné et initialisé prudemment après flash.
- Le mapping runout doit être contrôlé avec les deux branches de pins (LVGL et non-LVGL) : l'écran tactile utilise PA4, donc PA4 ne doit jamais être affecté au capteur filament.
- Le circuit de détection de perte secteur utilise PE3 dans la configuration LVGL de la carte. L'état électrique réel et les pull-up/pull-down doivent être validés sur carte.
- La ressource TFT/LVGL et les langues françaises/anglaises ne sont pas encore réintégrées/validées.
- Les options de pause/runout et la reprise PLR ont été activées en première passe seulement ; leurs séquences doivent être auditées avant d'être déclarées correctes.
- Aucune compilation PlatformIO ni aucun essai imprimante n'a été exécuté depuis cette session. Les actions de compilation/flash et les essais physiques resteront à effectuer sur PC/imprimante.

## Prochaine séquence

1. Porter fidèlement l'architecture dual MBL/ABL du constructeur vers les API Marlin 2.1.2.9 : données séparées, correction planner, G29, M420/M421/G42, M500/M501.
2. Auditer les appels à `auto_manu_level_sel`, fonctions sauvegarde/restauration et séquences LVGL.
3. Auditer le chemin d'exécution runout/pause et celui du PLR (PE3, SDIO, fichier recovery, arrêt et redémarrage).
4. Migrer les fichiers UI/assets en respectant l'organisation 2.1.2.9 et ne garder que français/anglais.
5. Vérifier le linker, l'environnement PlatformIO et la configuration de build STM32F103VET6.
6. Ajouter une procédure de compilation, de flash, de sauvegarde EEPROM et un protocole de validation sur banc.

## Règle de livraison

Ne jamais marquer une fonction comme « terminée » tant que la cohérence du code, sa compilation dans l'environnement visé et, lorsque nécessaire, un essai réel n'ont pas été réalisés.