# Geeetech Mizar S — suivi du portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris)

## Révisions et limites de l’audit

- Dépôt constructeur : [Geeetech3D/Mizar-S, main, d3656d6db4bb89f552f9afd3aec013db02a1cff9](https://github.com/Geeetech3D/Mizar-S/tree/d3656d6db4bb89f552f9afd3aec013db02a1cff9), source Marlin 2.0.7.
- Base du port : [Marlin 2.1.2.9, 7d300b42acadb3e7467e9f4b4fc03ebea592c2d0](https://github.com/MarlinFirmware/Marlin/tree/7d300b42acadb3e7467e9f4b4fc03ebea592c2d0).
- Code Mizar audité : [d81c6016694c6b01754f524dde5e877e401e144f](https://github.com/SounMayon/Marlin/tree/d81c6016694c6b01754f524dde5e877e401e144f).
- Workflow et contrôle d’assets vérifiés : [48e640393147aa69f561b7f90163733c02dee5ab](https://github.com/SounMayon/Marlin/tree/48e640393147aa69f561b7f90163733c02dee5ab).
- Branche de travail : mizar-s-marlin-2.1.2.9.
- Marlin main examiné : [d58bae7e9aa4a11688af55aa4d3d94bb5df227c7](https://github.com/MarlinFirmware/Marlin/tree/d58bae7e9aa4a11688af55aa4d3d94bb5df227c7).

Aucune modification n’a été apportée aux branches main ou bugfix-2.1.x.

## Comparaison des bases Git

À la révision de code d81, le port était 111 commits devant le tag 2.1.2.9, zéro derrière, avec le tag comme merge-base et 43 chemins modifiés ou ajoutés. Les commits suivants sur la branche concernent l’audit, la documentation et la livraison CI des assets; ils ne changent pas la base du firmware Mizar examinée. Au commit d’audit 30116c2afd8c468279dbe95b028b9300620bb1c5, la comparaison actualisée donne 117 commits d’avance, zéro derrière et toujours 43 chemins modifiés/ajoutés par rapport au tag.

Le Marlin main courant est divergent du tag 2.1.2.9 : 4 533 commits d’avance, 3 495 de retard, merge-base 082fce5e3e68c7b8657201e5de5adba0e4f86a33. La comparaison GitHub de main est limitée à 300 chemins. Il existe un conflit d’identifiant de carte : le port affecte 5068 à BOARD_GTM32_LCD35_A10, tandis que le Marlin main courant l’affecte à BOARD_MD_D301. Une migration vers main n’est pas incluse.

Le code Geeetech est destiné à l’écran propriétaire Mizar S. Le port remplace cet UI par le MKS LVGL de Marlin 2.1.2.9; ce n’est pas une fusion fichier par fichier.

## Carte et configuration

Les valeurs machine comparées conservent la carte GTM32/A10, le volume 255×255×260 mm, les capteurs thermiques 1/1, les pas par unité {80, 80, 400, 420}, EEPROM et ABL bilinéaire. La carte passe de l’identifiant 4038 du code constructeur à 5068 dans la branche 2.1.2.9.

Les broches retenues dans la configuration LVGL sont la sonde Z PC4, le runout PA15 actif-bas avec pull-up et la détection de perte secteur PE3 active-bas. En variante sans LVGL, PA4 est affecté à la fois au runout et à TOUCH_CS; ne pas utiliser cette variante sans corriger le conflit.

Le script et le linker réservent l’espace bootloader à 0x08007000, soit 28 Kio, et 4 Kio pour l’EEPROM flash. La compilation confirme que ces fichiers sont acceptés par le build; le bootloader réel et le câblage n’ont pas été contrôlés.

## Compilation et empaquetage CI vérifiés

Le workflow exécute pio run -e STM32F103VE_GTM32_maple. Le run [42401350884](https://github.com/SounMayon/Marlin/actions/runs/42401350884), sur le commit 30116c2afd8c468279dbe95b028b9300620bb1c5, a réussi. Il a produit l’artefact 13585597584 (7 985 239 octets).

Le journal confirme le téléchargement et la copie des ressources MKS, la création de firmware.bin, puis la vérification de 118 noms F(...) non commentés dans pic_manager.cpp. Tous les noms existent dans le dossier assets généré, avec comparaison insensible à la casse. L’artefact [mizar-s-marlin-2.1.2.9](https://github.com/SounMayon/Marlin/actions/runs/42400737794) inclut firmware.bin, firmware.elf et le dossier assets. Détails : [TFT_ASSETS.md](mizar-s/TFT_ASSETS.md).

Ce succès confirme le build CI et le contrôle des noms d’images à cette révision. Il ne valide ni la mise à jour de l’écran ni un démarrage, un capteur, une impression ou une reprise sur la machine.

## Nivellements ABL et MBL

Le port conserve deux grilles distinctes, un choix de mode Mizar et la persistance EEPROM des données et du mode. La revue statique des chemins d’arrêt et d’annulation a été faite : l’ABL s’arrête avant le point de mesure suivant et restaure le mode précédent; le MBL annule en restaurant sa copie de départ et sauvegarde le maillage terminé. Un mouvement de sonde déjà engagé n’est pas interrompu.

Écart de comportement à garder visible : le MBL du port utilise actuellement la grille Marlin 4×4, donc 16 points. Le manuel français fourni décrit un parcours manuel à 9 points; le journal V2.0.1 fourni indique que le manuel de nivellement est passé à 9 points dans la version V1.9.5, après un changement antérieur à 16 points dans V1.9.3. Le code Mizar 2.0.7 publié garde aussi GRID_MAX_POINTS_X=4 dans sa configuration générique, mais son écran manuel propriétaire et son journal de versions ne sont pas le même chemin que le G29 MBL du port. Le port conserve donc une différence de résolution intentionnelle; l’écran et la grille n’ont pas été testés ensemble.

Le manuel utilisateur décrit aussi les données manuelle et automatique comme persistantes et indique que la machine privilégie la dernière méthode utilisée. Le port améliore ce modèle en gardant deux maillages indépendants et en permettant de choisir celui qui est actif. Cette différence est voulue par le port, mais doit être vérifiée sur l’écran et par lecture/écriture EEPROM.

## Runout et reprise après perte secteur

- **Runout :** le handler LVGL lit PA15 actif-bas, applique un filtrage logiciel, met le fichier SD en pause, attend la fin des mouvements, parque la tête et permet la reprise manuelle. La correction du seuil de position couvre un événement au tout début du fichier. Ce handler ne lance pas automatiquement M600, chargement ni purge.
- Le manuel français indique la pause automatique en cas de rupture/fin de filament, le remplacement, puis la reprise depuis l’écran. L’archive V2.0.1 note toutefois que M600 a été ajouté dans V2.0.0; l’équivalence avec cette fonction plus récente du firmware constructeur n’est pas complète.
- **Perte secteur :** le code active PLR sur PE3 actif-bas; les données sont stockées séparément des réglages avec taille et CRC vérifiés. Aucun pull-up/pull-down interne n’est forcé sans confirmation du circuit. Le manuel exige de ne pas déplacer Z après la coupure et décrit la reprise d’un travail enregistré sur écran. La séquence de coupure, redémarrage, restauration SD et reprise n’a pas été exécutée sur la machine.

Le lien Pastebin raw fourni n’a pas pu être lu avec les outils d’accès disponibles. Le contenu du collage n’a pas été utilisé ni supposé. Le téléchargement Geeetech Support download_id=57 n’a pas été confirmé lors de l’audit antérieur.

## Langues et ressources d’écran

La branche limite l’UI à français/anglais et contient les chaînes des pages de nivellement dans ces deux langues. Le manuel Mizar S de 2022 décrit chinois/anglais; le choix français/anglais est donc une extension du port.

Le paquet d’images MKS est épinglé par le script PlatformIO de Marlin 2.1.2.9, contrôlé dans le workflow, et joint à l’artefact CI. L’archive Geeetech fournie n’est pas utilisée comme jeu d’assets MKS; voir [TFT_ASSETS.md](mizar-s/TFT_ASSETS.md).

## État actuel

**Build et contrôle statique des ressources réussis en CI. Port non validé sur la machine.** Les écarts MBL 16/9 points et l’absence de M600 automatique dans le chemin runout sont documentés; ils ne doivent pas être décrits comme une parité parfaite avec le firmware constructeur V2.0.1.

Restent à vérifier sur une Mizar S : mise à jour et rendu TFT, écrans FR/EN, parcours ABL et MBL (dont sauvegarde séparée), polarité réelle de PA15 et reprise après runout, circuit PE3 et reprise PLR complète, migration EEPROM et impression. Le firmware GTM32Source.bin de 2023 contenu dans l’archive n’est pas le build du port et n’a pas été utilisé.