# Portage Geeetech Mizar S vers Marlin 2.1.2.9

## Base logicielle verrouillée

- Dépôt de travail : `SounMayon/Marlin`
- Branche de travail : `mizar-s-marlin-2.1.2.9`
- Base : tag officiel Marlin `2.1.2.9`
- Commit exact : `7d300b42acadb3e7467e9f4b4fc03ebea592c2d0`
- Dépôt constructeur : `SounMayon/Mizar-S`
- Révision constructeur examinée : `d3656d6db4bb89f552f9afd3aec013db02a1cff9`

Aucune branche existante (`main` ou `bugfix-2.1.x`) n'a été modifiée. Cette branche de portage a été créée directement depuis le commit Marlin 2.1.2.9 vérifié.

## Cible matérielle

- Geeetech Mizar S
- Carte annoncée par l'écran : `GT-FM-MIZAR6V0.0`
- MCU identifié dans la définition constructeur : STM32F103VET6
- Volume : 255 x 255 x 260 mm
- Double Z, BLTouch, écran tactile couleur MKS/Geeetech avec stockage SPI externe.

## Fonctions à préserver / porter

1. Définition de carte et brochage exacts.
2. Écran tactile MKS UI, ressources graphiques et menus.
3. Nivellement automatique bilinéaire BLTouch.
4. Nivellement manuel guidé et coexistence/sélection auto-manuel de la grille.
5. Détection de fin de filament et séquence de reprise après réinsertion.
6. Power Loss Recovery (PLR), persistance et reprise après redémarrage.
7. EEPROM, paramètres et fonctions machine réellement présentes sur la Mizar S.
8. Interface française et anglaise uniquement.

## Constats source déjà vérifiés

- Le dépôt constructeur contient `Marlin2.0.7/Marlin`, la définition `pins_GTM32_LCD35_A10.h`, la configuration, le code MKS UI, les ressources et un environnement PlatformIO `geeetech_lcd35`.
- L'identifiant constructeur dans son `boards.h` est **4038** pour `BOARD_GTM32_LCD35_A10` (STM32F103VET6). Ne pas utiliser 5068.
- La définition de broches présente plusieurs variantes conditionnelles. Elle contient notamment PA15 pour `MT_DET_1_PIN`, PA8 pour le servo BLTouch, PC4 pour le signal de sonde, PA4 pour TOUCH_CS, mais aussi des définitions `FIL_RUNOUT_PIN PA4` dans une branche conditionnelle. Il faut résoudre précisément les conditions et la configuration réelle avant de fixer le pin de runout : PA4 ne doit pas être affecté au runout si l'écran tactile l'utilise.
- Le fichier constructeur `Configuration.h` active simultanément `AUTO_BED_LEVELING_BILINEAR` et `MESH_BED_LEVELING`; il déclare toutefois `FILAMENT_RUNOUT_SENSOR` commenté. La configuration réellement utilisée doit donc être rapprochée des macros de l'UI et du firmware installé, sans déduire l'activation runout de ce seul fichier.
- `Configuration_adv.h` active `POWER_LOSS_RECOVERY`, `POWER_LOSS_STORAGE_EEPROM` et `POWER_LOSS_STORAGE_Z_VALUE`, ainsi que deux moteurs Z et `Z2_USE_ENDSTOP _ZMAX_`.
- `bedlevel.h` constructeur introduit une sélection `auto_manu_level_sel` entre les tableaux ABL et MBL. La coexistence des deux systèmes doit être portée intégralement et contrôlée dans tous les appels, pas seulement en levant le sanity check.
- Le constructeur utilise une UI sous `src/lcd/extui/lib/mks_ui`; Marlin 2.1.2.9 doit être vérifié pour son emplacement, ses API et ses dépendances avant intégration.
- Le constructeur fournit `geeetech_lcd35.py` et `geeetech_lcd35.ld`; leur équivalence avec les scripts, environnements et linker de la version cible reste à démontrer.
- Le fichier de langue française constructeur existe. Il faut comparer ses clés à celles de la version cible et garder anglais/français sans importer automatiquement les autres langues.

## Points à auditer avant modification fonctionnelle

- Vérifier le chemin exact du capteur de filament réellement utilisé par le firmware livré (PA15 / macros MKS UI / éventuelle autre branche de broches) et sa polarité électrique.
- Vérifier le pin et la polarité de détection de coupure secteur, car plusieurs définitions conditionnelles apparaissent dans le fichier de pins.
- Cartographier tous les usages de `auto_manu_level_sel`, les fonctions de sauvegarde/restauration des deux maillages et les commandes G-code correspondantes.
- Tracer le flux runout -> pause/M600 -> dialogue écran -> insertion -> purge -> reprise/annulation.
- Tracer l'écriture, la validation, la détection et la restauration des données PLR, y compris SD, écran et EEPROM.
- Auditer les 107 images environ, les noms attendus par le gestionnaire d'images, le format réel et la procédure de mise à jour SPI avant de décider quels assets sont nécessaires.
- Comparer les configurations de compilation et les scripts PlatformIO sans présumer que l'environnement MKS Robin Nano est interchangeable.

## Règle de validation

Chaque adaptation sera associée à ses sources d'origine, à la justification de portage et à une vérification statique. Aucune compilation locale, aucun test électrique et aucun essai sur imprimante ne seront déclarés réussis sans exécution effective. La validation finale nécessitera la compilation sur le poste utilisateur puis des essais contrôlés sur la machine.
