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

## Avancement GitHub — première intégration carte

Commit de base conservé dans l'historique de la branche. Les premiers fichiers d'intégration ont été ajoutés :

- `Marlin/src/pins/stm32f1/pins_GTM32_LCD35_A10.h` : définition de broches constructeur importée et identifiée Mizar S.
- `Marlin/src/core/boards.h` : identifiant Marlin cible `BOARD_GTM32_LCD35_A10 = 5068`.
- `Marlin/src/pins/pins.h` : sélection du fichier de broches pour cette carte.
- `ini/stm32f1.ini` : environnement `geeetech_lcd35` ajouté en s'appuyant sur la base STM32F103VE avec offset applicatif 0x7000 et timer servo TIM1, cohérent avec PA8 du BLTouch.

**Pourquoi 5068 et pas 4038 :** le code constructeur Marlin 2.0.7.2 utilise l'ID 4038. Dans le tag 2.1.2.9, la plage 5000+ est utilisée pour les cartes STM32F1 et 5038 est déjà attribué à Chitu3D V9. Le premier ID libre immédiatement après les identifiants existants 5067 est 5068. Le nom symbolique reste `BOARD_GTM32_LCD35_A10`.

## Réglages machine relevés dans la configuration constructeur

- `SERIAL_PORT 1`, `BAUDRATE 250000`.
- `X_BED_SIZE 255`, `Y_BED_SIZE 255`, `Z_MAX_POS 260`.
- BLTouch activé, `NOZZLE_TO_PROBE_OFFSET { 0.0, 0.0, 0.20 }`.
- ABL bilinéaire 4 x 4 et MBL sont tous deux activés dans la configuration constructeur.
- EEPROM activée et SD support activé.
- `TFT_LVGL_UI` est activé dans la configuration constructeur.
- Deux moteurs Z et second Z utilisant l'endstop Z_MAX.
- PLR activé avec stockage EEPROM et stockage de la valeur Z.

Attention : la configuration source commente `FILAMENT_RUNOUT_SENSOR` alors que le firmware installé a présenté un comportement de runout en utilisation réelle. L'interface MKS possède son propre chemin `MT_DET_1_PIN`; ce chemin doit être intégré et réconcilié avec le runout natif Marlin 2.1.2.9, sans activer arbitrairement le mauvais pin ou la mauvaise polarité.

## Compatibilité écran

Marlin 2.1.2.9 contient déjà une implémentation MKS LVGL sous `Marlin/src/lcd/extui/mks_ui`, alors que le code Geeetech 2.0.7.2 est sous `Marlin/src/lcd/extui/lib/mks_ui`. Il ne faut donc pas écraser la version cible avec l'ancien répertoire. Le travail doit porter sur les extensions Mizar spécifiques (sélection des deux nivellements, commandes de l'écran, reprise runout/PLR, textes et ressources manquantes) après comparaison fichier par fichier.

Le linker `geeetech_lcd35.ld` du constructeur et `mks_robin_nano.ld` de Marlin 2.1.2.9 ont le même contenu observé, avec origine ROM `0x08007000`. L'environnement cible hérite donc de la configuration de base STM32F103VE à offset `0x7000`; le script custom du constructeur n'a pas été copié à ce stade.

## Audit approfondi du double nivellement

Le firmware constructeur n'active pas simplement deux options indépendantes. Il modifie le modèle de données :

- ABL bilinéaire : maillage et coordonnées bilinéaires propres.
- MBL manuel : maillage distinct et coordonnées propres.
- `auto_manu_level_sel` sélectionne le maillage utilisé par le planificateur.
- `leveling_is_valid()`, `set_bed_leveling_enabled()`, `Planner::apply_leveling()`, `Planner::unapply_leveling()`, `M420`, `G29`, la sauvegarde EEPROM et l'écran tactile utilisent cette sélection.
- Les données ABL/MBL sont aussi sauvegardées dans les paramètres MKS SPI (`bk_abl_*`, `bk_mbl_z_value`).

Les premiers fichiers confirmés comme concernés sont :
`feature/bedlevel/bedlevel.h/.cpp`,
`feature/bedlevel/abl/abl.h`,
`feature/bedlevel/mbl/mesh_bed_leveling.h`,
`module/planner.cpp`,
`module/settings.cpp`,
`gcode/bedlevel/M420.cpp`,
`gcode/bedlevel/abl/G29.cpp`,
`gcode/bedlevel/mbl/G29.cpp`,
`MarlinCore.cpp`,
et les vues MKS `draw_autoLevel.cpp`, `draw_manuaLevel.cpp`.

**Défaut trouvé dans le code constructeur :** dans `leveling_data_backup()` et `leveling_data_restore()`, les coordonnées Y de `bilinear_start` et `bilinear_grid_spacing` sont assignées à partir de X au lieu de Y. Le même défaut apparaît dans la sauvegarde/restauration des valeurs MKS SPI. Cette erreur ne sera pas recopiée. Le portage conservera X et Y séparément et sera vérifié avec des valeurs de départ/espacement non identiques.

## Runout et reprise

- Le firmware constructeur a un chemin MKS LVGL utilisant `MT_DET_1_PIN` et un chemin standard Marlin utilisant `FIL_RUNOUT_PIN`; la configuration constructeur n'active pas le `FILAMENT_RUNOUT_SENSOR` natif.
- Le runout natif Marlin 2.1.2.9 évite explicitement de déclencher à nouveau l'événement lorsqu'une pause est déjà en cours. Cette protection doit être prise en compte dans l'intégration MKS.
- L'interface MKS contient des écrans et états de changement de filament, mais la version 2.1.2.9 a déjà évolué. Le code d'écran ne sera pas remplacé en bloc par la version ancienne.
- Le PLR constructeur appelle directement le code MKS et le stockage EEPROM ; le PLR Marlin 2.1.2.9 utilise son propre fichier de récupération et ses propres API. Il faut préserver la compatibilité de l'écran sans dupliquer deux systèmes de récupération concurrents.

## Compilation — état

- Le nouvel environnement `geeetech_lcd35` est déclaré dans `ini/stm32f1.ini`.
- Il hérite de `STM32F103VE_robin`, qui définit l'offset de bootloader `0x7000`; il remplace le timer servo TIM2 par TIM1, adapté à la sortie BLTouch PA8.
- Les fichiers linker constructeur et MKS Robin Nano ont été comparés et sont identiques.
- Aucune compilation PlatformIO n'a été exécutée à distance. L'environnement et les flags restent à valider par compilation utilisateur.
