# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris)

## Révisions de l’audit

- Dépôt constructeur : [Geeetech3D/Mizar-S, main, d3656d6db4bb89f552f9afd3aec013db02a1cff9](https://github.com/Geeetech3D/Mizar-S/tree/d3656d6db4bb89f552f9afd3aec013db02a1cff9), code Marlin 2.0.7.
- Base Marlin du port : [tag 2.1.2.9, 7d300b42acadb3e7467e9f4b4fc03ebea592c2d0](https://github.com/MarlinFirmware/Marlin/tree/7d300b42acadb3e7467e9f4b4fc03ebea592c2d0).
- Révision du port examinée : [mizar-s-marlin-2.1.2.9, d81c6016694c6b01754f524dde5e877e401e144f](https://github.com/SounMayon/Marlin/tree/d81c6016694c6b01754f524dde5e877e401e144f).
- Marlin `main` examiné : [d58bae7e9aa4a11688af55aa4d3d94bb5df227c7](https://github.com/MarlinFirmware/Marlin/tree/d58bae7e9aa4a11688af55aa4d3d94bb5df227c7).

Aucune modification n’a été faite sur `main` ni `bugfix-2.1.x`.

## Résultat de la comparaison Git

- La branche du port est **111 commits devant** le tag Marlin 2.1.2.9, **0 derrière**; le merge-base est exactement le tag. Elle comporte 43 chemins modifiés/ajoutés par rapport à ce tag.
- Ces différences portent notamment sur la configuration et la carte GTM32, les deux maillages ABL/MBL, la reprise après perte secteur, les écrans MKS LVGL, le build CI et les notes de portage.
- Le Marlin `main` courant est **divergent** du tag 2.1.2.9 : 4 533 commits d’avance, 3 495 de retard, merge-base `082fce5e3e68c7b8657201e5de5adba0e4f86a33`. L’API de comparaison ne retourne que 300 chemins pour cette comparaison.
- Conflit concret de fusion : le port attribue l’identifiant de carte `5068` à `BOARD_GTM32_LCD35_A10`, libre dans sa base 2.1.2.9; Marlin `main` utilise maintenant `5068` pour `BOARD_MD_D301`. Le port doit rester fondé sur 2.1.2.9 tant qu’une migration dédiée ne réalloue pas cet identifiant et ne reprend pas ses modifications une par une.
- Le README constructeur indique que son code Marlin 2.0.7 est destiné à l’écran Mizar S d’origine. Le port est donc une adaptation à Marlin 2.1.2.9, pas une copie fichier à fichier.

## Configuration et carte comparées

La configuration 2.1.2.9 conserve les réglages machine du profil constructeur examinés dans `Configuration.h` : carte A10, volume 255×255×260 mm, capteurs thermiques 1/1, pas par unité `{80, 80, 400, 420}`, ABL bilinéaire et MBL avec sauvegarde EEPROM. Le numéro de carte change de 4038 dans la base 2.0.7 constructeur à 5068 dans le port 2.1.2.9.

Les broches clés sont conservées dans la carte dédiée : sonde Z PC4; capteur runout LVGL PA15, actif-bas avec pull-up; entrée de détection de perte secteur PE3, active-bas. En variante sans LVGL, PA4 apparaît à la fois pour le runout et pour le tactile; cette variante est signalée comme inutilisable sans correction.

Le script et le linker du port reprennent l’adresse constructeur `0x08007000`, le décalage bootloader de 28 Kio et la réserve flash EEPROM de 4 Kio. Leur compilation est vérifiée, mais le contenu du bootloader et le câblage réel n’ont pas été contrôlés sur la machine.

## Compilation CI vérifiée

Le workflow GitHub Actions exécute `pio run -e STM32F103VE_GTM32_maple`.

- Run [42394435321](https://github.com/SounMayon/Marlin/actions/runs/42394435321), commit `d81c6016694c6b01754f524dde5e877e401e144f) : **succès**.
- L’artefact `mizar-s-marlin-2.1.2.9` a été publié; il contient `firmware.bin`.
- Ce résultat confirme la compilation CI de cette révision. Il ne valide ni le démarrage sur la carte réelle, ni l’écran, ni les capteurs, ni une impression.

## Nivellement ABL/MBL

La sélection Mizar conserve deux jeux de données distincts : maillage ABL bilinéaire et grille MBL manuelle 4×4. Les G-code, le planificateur et l’EEPROM choisissent le bon jeu selon le mode. Les parcours d’annulation/restauration et d’arrêt de l’ABL ont été relus statiquement. L’ABL s’arrête avant le point de mesure suivant; une mesure déjà engagée n’est pas interrompue.

Le MBL est sauvegardé après la séquence guidée; l’annulation restaure sa copie. Ces chemins n’ont pas été essayés sur l’écran ou la machine.

## Runout, reprise secteur et langues

- **Runout :** le MKS UI lit PA15 en actif-bas, avec pull-up et filtrage logiciel. Il met en pause le fichier SD, attend la fin des mouvements, parque la tête puis permet une reprise manuelle. La correction du seuil de position couvre un runout au tout début du fichier. Ce chemin ne déclenche pas M600, purge ou chargement automatique. Polarité et séquence de reprise restent à essayer sur la carte.
- **Perte secteur :** PLR activé sur PE3 actif-bas, stockage EEPROM séparé avec CRC et stockage de la position Z physique. Aucun biais interne n’est activé tant que le circuit n’est pas confirmé. La séquence coupure, redémarrage, détection, restauration SD et reprise n’a pas été testée.
- **Langues :** le code limite le choix à français/anglais et contient les chaînes de nivellement correspondantes. Les images nécessaires à l’écran de choix ne sont pas complètes dans les ressources GitHub du constructeur; voir [TFT_ASSETS.md](mizar-s/TFT_ASSETS.md).

### Correction du comptage d’images

Une note précédente indiquait que les 120 noms d’images du port correspondaient tous aux JPG de `SounMayon/Mizar-S`. C’était inexact : le dépôt de ce fork est à la même révision source que Geeetech et ne contient que les 107 images constructeur. Le gestionnaire MKS du port demande 120 images distinctes; 61 ont un JPG constructeur homonyme et 59 n’en ont pas. Le dépôt du port ne contient pas le paquet TFT `assets/` ni les binaires d’icônes. Le [README Geeetech](https://github.com/Geeetech3D/Mizar-S/blob/d3656d6db4bb89f552f9afd3aec013db02a1cff9/README.md) documente la conversion `Bin_565`; la procédure Wiki demande le binaire firmware et le dossier `assets` à la racine TF, puis de refaire le nivellement.

Le lien Support `download_id=57` et le Pastebin indiqués n’étaient pas accessibles pendant l’audit; leur contenu n’est pas confirmé. Aucun essai matériel n’est disponible pour l’écran, le runout, la perte secteur, la migration EEPROM ou l’impression.

## État et travaux restant avant validation complète

**État : compilation CI réussie; port logiciel non validé matériellement et ressources TFT incomplètes. Ne pas le présenter comme un firmware prêt à flasher/validé sur Mizar S.**

1. Obtenir et inspecter le paquet constructeur `assets/`; satisfaire les 120 références du port, convertir en Bin_565 et contrôler les noms/dimensions.
2. Vérifier sur l’écran la mise à jour TFT, les icônes, le choix français/anglais et les pages ABL/MBL.
3. Essayer ABL et MBL : entrée, arrêt, annulation, fin, activation et persistance des deux maillages.
4. Vérifier le runout PA15 actif-bas, la pause en début de fichier et la reprise manuelle.
5. Vérifier PE3 et une reprise complète après perte secteur avec impression depuis SD.
6. Documenter la procédure de flash et de migration EEPROM, puis valider l’ensemble sur banc et imprimante.

